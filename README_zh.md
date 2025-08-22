# QClipboard

一个基于Qt的跨平台剪贴板管理工具，支持多设备同步。可以从剪贴板历史记录中选择对应的项进行粘贴。它提供了一种查看剪贴板历史记录和快速粘贴任何以前复制的内容的简便方式。

## 特性

- 📋 本地剪贴板管理
- 🔄 多设备数据实时同步（文本、图片）
- 🖼️ 图片自动同步下载和剪贴板更新
- ⚡ 全局快捷键支持，支持自定义（默认`Alt + V`）
- 🎯 系统托盘运行

## 构建要求

- C++ 20
- Qt 6.0+

## 构建

```bash
cmake -B build
cmake --build
```

## 配置

在配置文件中设置服务器信息：
```json
{
  "server": {
    "url": "https://your-sync-server.com",
    "user": "your-email@example.com",
    "password": "your-password",
    "device_name": "My Device"
  },
  "shortcut": "Alt+V"
}
```
## 贡献

欢迎贡献 - 提交 issue 和 PR。

## 许可证

[MIT License](LICENSE)