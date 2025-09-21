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


def build_package(build_dir, exe_name):
    print("Building NSIS package...")
    return build_nsis_installer(build_dir, exe_name)


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


def build_nsis_installer(build_dir, exe_name):
    """Build NSIS installation package"""
    # Get project root directory
    project_root = Path(__file__).parent.parent

    # Create installer directory
    installer_dir = build_dir.parent / "output"
    installer_dir.mkdir(exist_ok=True)

    # Copy executable files and dependencies to installer directory
    app_dir = installer_dir / "src"
    app_dir.mkdir(exist_ok=True)

    shutil.copy(project_root / "scripts/nsis_package.nsi", installer_dir)
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
    nsis_script_path = installer_dir / "nsis_package.nsi"

    # Execute NSIS compilation
    cmd = f'"{nsis_compiler}" /INPUTCHARSET UTF8 "{nsis_script_path}"'
    if not run_command(cmd, installer_dir):
        return False

    print("NSIS installation package build completed!")
    return True


def main():
    print("=== QClipboard package tool ===")

    parser = argparse.ArgumentParser(description='Package tool into an installer.')
    parser.add_argument('--output-dir', '-o', type=str, default='build',
                        help='The binary output directory')
    parser.add_argument('--app-name', '-e', type=str, default='QClipboard.exe',
                        help='Name of the executable file')

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

    if not build_package(build_dir, args.app_name):
        print("Packaging failed!")
        return 1

    print("Packaging completed!")
    return 0


if __name__ == "__main__":
    sys.exit(main())
