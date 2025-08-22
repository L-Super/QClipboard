# QClipboard
[中文](README_zh.md)

A cross-platform clipboard management tool based on Qt, supporting multi-device synchronization. You can select items from clipboard history for pasting. It provides an easy way to view clipboard history and quickly paste any previously copied content.

## Features

- 📋 Local clipboard management
- 🔄 Real-time multi-device data synchronization (text, images)
- 🖼️ Automatic image sync, download, and clipboard update
- ⚡ Global hotkey support, customizable(default is `Alt + V`)
- 🎯 Runs in system tray

## Build Requirements

- C++ 20
- Qt 6.0+

## Build

```bash
cmake -B build
cmake --build
```

## Configuration

Set server information in the configuration file:
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

## Contributing

Contributions welcome - submit issues and pull requests.

## License

[MIT License](LICENSE)