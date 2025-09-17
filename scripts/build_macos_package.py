#!/usr/bin/env python3
import os
import sys
import subprocess
import argparse
from pathlib import Path
import shutil
import tempfile

def run_command(cmd, cwd=None, check=True):
    """Execute command and return result"""
    print(f"Executing command: {cmd}")
    try:
        result = subprocess.run(cmd, shell=True, cwd=cwd, capture_output=True, text=True, check=check)
        if result.returncode != 0:
            print(f"Command execution failed: {result.stderr}")
            return False
        print(f"Command executed successfully: {result.stdout}")
        return True
    except subprocess.CalledProcessError as e:
        print(f"Command execution failed: {e}")
        if check:
            raise
        return False

def create_dmg_package(build_dir, app_name, project_root):
    """Create DMG package for macOS"""
    print("Creating DMG package...")
    
    # Check if we have a .app bundle
    app_bundle_path = build_dir / "src" / f"{app_name}.app"
    if not app_bundle_path.exists():
        print(f"Error: Application bundle {app_bundle_path} not found")
        print("Make sure you built the project with MACOSX_BUNDLE enabled")
        return False
    
    # Create temporary directory for DMG creation
    dmg_work_dir = build_dir / "dmg_work"
    if dmg_work_dir.exists():
        shutil.rmtree(dmg_work_dir)
    dmg_work_dir.mkdir(exist_ok=True)
    
    # Copy app bundle to working directory
    app_copy_path = dmg_work_dir / f"{app_name}.app"
    shutil.copytree(app_bundle_path, app_copy_path)
    
    # Create symbolic link to Applications folder
    apps_link = dmg_work_dir / "Applications"
    apps_link.symlink_to("/Applications")
    
    # Copy background image if available
    background_src = project_root / "src" / "resources" / "mac" / "background.png"
    if background_src.exists():
        background_dir = dmg_work_dir / ".background"
        background_dir.mkdir(exist_ok=True)
        shutil.copy2(background_src, background_dir / "background.png")
    
    # Create DMG
    dmg_name = f"{app_name}.dmg"
    dmg_path = build_dir / dmg_name
    
    # Use create-dmg if available, otherwise use hdiutil
    if shutil.which("create-dmg"):
        # Use create-dmg tool
        cmd = [
            "create-dmg",
            "--volname", app_name,
            "--window-pos", "200", "120",
            "--window-size", "800", "400",
            "--icon-size", "100",
            "--icon", f"{app_name}.app", "100", "100",
            "--hide-extension", f"{app_name}.app",
            "--app-drop-link", "300", "100",
            str(dmg_path),
            str(dmg_work_dir)
        ]
        if not run_command(" ".join(cmd)):
            print("Failed to create DMG with create-dmg")
            return False
    else:
        # Use hdiutil (built-in macOS tool)
        print("Using hdiutil to create DMG...")
        
        # Create temporary disk image
        temp_dmg = build_dir / "temp.dmg"
        if temp_dmg.exists():
            temp_dmg.unlink()
        
        # Create initial DMG
        cmd = f"hdiutil create -srcfolder {dmg_work_dir} -volname {app_name} -fs HFS+ -size 100m {temp_dmg}"
        if not run_command(cmd):
            print("Failed to create temporary DMG")
            return False
        
        # Convert to compressed DMG
        if dmg_path.exists():
            dmg_path.unlink()
            
        cmd = f"hdiutil convert {temp_dmg} -format UDZO -imagekey zlib-level=9 -o {dmg_path}"
        if not run_command(cmd):
            print("Failed to compress DMG")
            # Clean up temp file
            if temp_dmg.exists():
                temp_dmg.unlink()
            return False
        
        # Clean up temp file
        if temp_dmg.exists():
            temp_dmg.unlink()
    
    # Clean up working directory
    if dmg_work_dir.exists():
        shutil.rmtree(dmg_work_dir)
    
    if not dmg_path.exists():
        print("Error: DMG file was not created successfully")
        return False
    
    print("DMG creation completed!")
    return True

def notarize_app(build_dir, app_name, apple_id, team_id, app_password):
    """Notarize the macOS app"""
    print("Notarizing app...")
    
    # Create zip archive of the app
    app_bundle_path = build_dir / "src" / f"{app_name}.app"
    zip_path = build_dir / f"{app_name}.zip"
    
    if zip_path.exists():
        zip_path.unlink()
    
    # Create zip
    cmd = f"ditto -c -k --keepParent {app_bundle_path} {zip_path}"
    if not run_command(cmd):
        print("Failed to create zip archive for notarization")
        return False
    
    # Submit for notarization
    cmd = f"xcrun notarytool submit {zip_path} --apple-id {apple_id} --team-id {team_id} --password {app_password} --wait"
    if not run_command(cmd):
        print("Failed to submit app for notarization")
        return False
    
    # Staple the notarization ticket
    cmd = f"xcrun stapler staple {app_bundle_path}"
    if not run_command(cmd):
        print("Failed to staple notarization ticket")
        return False
    
    print("App notarization completed!")
    return True

def create_pkg_installer(build_dir, app_name, project_root):
    """Create PKG installer for macOS"""
    print("Creating PKG installer...")
    
    # Check if we have a .app bundle
    app_bundle_path = build_dir / "src" / f"{app_name}.app"
    if not app_bundle_path.exists():
        print(f"Error: Application bundle {app_bundle_path} not found")
        return False
    
    # Create component package
    pkg_path = build_dir / f"{app_name}.pkg"
    
    # Use pkgbuild to create the package
    identifier = f"com.QClipboard.{app_name}"
    cmd = f"pkgbuild --root {app_bundle_path.parent} --identifier {identifier} --version 1.0.4.0 --install-location /Applications {pkg_path}"
    
    if not run_command(cmd):
        print("Failed to create PKG installer")
        return False
    
    print("PKG installer creation completed!")
    return True

def main():
    print("=== QClipboard macOS package tool ===")
    
    parser = argparse.ArgumentParser(description='Package tool for macOS platforms.')
    parser.add_argument('--output-dir', '-o', type=str, default='build', 
                        help='Build directory (relative to project root)')
    parser.add_argument('--app-name', '-a', type=str, default='QClipboard',
                        help='Name of the application')
    parser.add_argument('--format', '-f', type=str, choices=['dmg', 'pkg', 'both'], default='dmg',
                        help='Package format to create')
    parser.add_argument('--notarize', '-n', action='store_true',
                        help='Notarize the app (requires Apple Developer credentials)')
    parser.add_argument('--apple-id', type=str, help='Apple ID for notarization')
    parser.add_argument('--team-id', type=str, help='Team ID for notarization')
    parser.add_argument('--app-password', type=str, help='App-specific password for notarization')
    
    args = parser.parse_args()
    
    project_root = Path(__file__).parent.parent
    if not (project_root / "CMakeLists.txt").exists():
        print("Error: Please run this script in the project root directory")
        return 1
    
    # Resolve build directory path
    build_dir = Path(args.output_dir)
    if not build_dir.is_absolute():
        build_dir = project_root / build_dir
    
    if not build_dir.exists():
        print(f"Error: Build directory does not exist: {build_dir} Please run CMake build first")
        return 1
    
    # Check if we're running on macOS
    if sys.platform != "darwin":
        print("Warning: This script is designed to run on macOS. Some features may not work correctly on other platforms.")
    
    success = True
    
    # Create packages based on format
    if args.format in ['dmg', 'both']:
        if not create_dmg_package(build_dir, args.app_name, project_root):
            success = False
    
    if args.format in ['pkg', 'both']:
        if not create_pkg_installer(build_dir, args.app_name, project_root):
            success = False
    
    # Notarize if requested
    if args.notarize and success:
        if not args.apple_id or not args.team_id or not args.app_password:
            print("Error: Apple ID, Team ID, and App Password are required for notarization")
            success = False
        elif not notarize_app(build_dir, args.app_name, args.apple_id, args.team_id, args.app_password):
            success = False
    
    if not success:
        print("Packaging failed!")
        return 1
    
    print("macOS packaging completed!")
    return 0

if __name__ == "__main__":
    sys.exit(main())