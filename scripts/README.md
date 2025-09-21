# QClipboard 打包脚本

本目录包含了用于构建 QClipboard 安装包的脚本。

## 脚本说明

### `build_windows_package.py` - Windows NSIS 安装包构建脚本
专门用于构建 Windows 平台的 NSIS 安装包。

**使用方法：**

```bash
python scripts/build_windows_package.py --output-dir build/src --app-name QClipboard.exe
```

**前置要求：**
- 已安装 NSIS (https://nsis.sourceforge.io/Download)
- 已完成 CMake 构建

### `build_linux_package.py` - Linux 安装包构建脚本
专门用于构建 Linux 平台的 AppImage 安装包。

**使用方法：**
```bash
python3 scripts/build_linux_package.py --output-dir build/src --app-name QClipboard ----format appimage
```

**前置要求：**
- 已安装依赖: `sudo apt-get install -y libfuse2 patchelf`
- 已完成 CMake 构建

**TODO:**

- [ ]  支持 deb 格式

## 构建流程

### 本地构建

1. **构建项目**
   ```bash
   cmake -B build
   cmake --build build
   ```

2. **构建安装包**
   ```bash
   # 在项目根目录使用平台脚本
   python scripts/build_windows_package.py --output-dir build/src --app-name QClipboard.exe # Windows
   python3 scripts/build_linux_package.py --output-dir build/src --app-name QClipboard ----format appimage # Linux
   ```

### CI/CD构建

GitHub Actions会自动：
1. 安装必要的依赖（Qt、NSIS等）
2. 构建项目
3. 运行测试
4. 构建安装包
5. 上传构建产物

## 关于软件版本信息

Windows NSIS 脚本会从 exe 属性中读取版本信息，Linux Python脚本会自动从 `src/version.h` 文件中读取版本信息。
