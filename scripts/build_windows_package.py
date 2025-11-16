#!/usr/bin/env python3
import os
import sys
import subprocess
import argparse
from pathlib import Path
import shutil


def run_command(cmd, cwd=None):
    """Execute command and return result"""
    print(f"Executing command: {cmd}")
    result = subprocess.run(cmd, shell=True, cwd=cwd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Command execution failed: {result.stderr}")
        return False
    print(f"Command executed successfully: {result.stdout}")
    return True


def build_package(build_dir, exe_name, variant):
    print(f"Building NSIS package for {variant} version...")
    return build_nsis_installer(build_dir, exe_name, variant)


def find_nsis_compiler():
    """Find NSIS compiler"""
    possible_paths = [
        r"C:\Program Files\NSIS\makensis.exe",
        r"C:\Program Files (x86)\NSIS\makensis.exe",
        "makensis.exe"  # If in PATH
    ]

    for path in possible_paths:
        if os.path.exists(path):
            return path

    return None


def build_nsis_installer(build_dir, exe_name, variant):
    """Build NSIS installation package"""
    # Get project root directory
    project_root = Path(__file__).parent.parent

    # Create installer directory
    installer_dir = build_dir.parent / "output"
    installer_dir.mkdir(exist_ok=True)

    # Copy executable files and dependencies to installer directory
    app_dir = installer_dir / "src"
    if app_dir.exists():
        shutil.rmtree(app_dir)
    app_dir.mkdir(exist_ok=True)

    # Copy NSIS script and modify output filename based on variant
    nsis_script_path = installer_dir / "nsis_package.nsi"
    shutil.copy(project_root / "scripts/nsis_package.nsi", nsis_script_path)
    
    # Read and modify NSIS script to change output filename only
    with open(nsis_script_path, 'r', encoding='utf-8') as f:
        nsis_content = f.read()
    
    # Modify only the OutFile line based on variant
    if variant == "standalone":
        new_outfile = 'OutFile "QClipboard_${PRODUCT_VERSION}_x64_Standalone_Setup.exe"'
    else:  # sync
        new_outfile = 'OutFile "QClipboard_${PRODUCT_VERSION}_x64_Sync_Setup.exe"'
    
    # Replace the OutFile line in NSIS script
    lines = nsis_content.split('\n')
    for i, line in enumerate(lines):
        if line.strip().startswith('OutFile'):
            lines[i] = new_outfile
            break
    
    nsis_content = '\n'.join(lines)
    
    with open(nsis_script_path, 'w', encoding='utf-8') as f:
        f.write(nsis_content)

    shutil.copy(project_root / "src/resources/win/icon.ico", installer_dir)

    # Copy main program
    shutil.copy2(build_dir / exe_name, app_dir / exe_name)

    # Copy dependencies
    other_deps = list(build_dir.glob("*.dll"))
    for dep in other_deps:
        shutil.copy2(dep, app_dir)

    # Copy Qt resource files
    qt_resources = ["generic", "iconengines", "imageformats", "networkinformation", "platforms", "styles", "tls"]
    for dir in qt_resources:
        shutil.copytree(build_dir / dir, app_dir / dir, dirs_exist_ok=True)

    # Find NSIS compiler
    nsis_compiler = find_nsis_compiler()
    if not nsis_compiler:
        print("Error: NSIS compiler not found, please install NSIS")
        return False

    # Build installation package
    print(f"Building installer for {variant} version...")

    # Execute NSIS compilation
    cmd = f'"{nsis_compiler}" /INPUTCHARSET UTF8 "{nsis_script_path}"'
    if not run_command(cmd, installer_dir):
        return False

    print(f"NSIS installation package for {variant} version build completed!")
    return True


def main():
    print("=== QClipboard package tool ===")

    parser = argparse.ArgumentParser(description='Package tool into an installer.')
    parser.add_argument('--output-dir', '-o', type=str, default='build',
                        help='The binary output directory')
    parser.add_argument('--app-name', '-e', type=str, default='QClipboard.exe',
                        help='Name of the executable file')
    parser.add_argument('--variant', '-v', type=str, choices=['standalone', 'sync'], 
                        default='standalone', help='Build variant: standalone or sync')

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

    if not build_package(build_dir, args.app_name, args.variant):
        print("Packaging failed!")
        return 1

    print(f"Packaging for {args.variant} version completed!")
    return 0


if __name__ == "__main__":
    sys.exit(main())