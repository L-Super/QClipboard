# Floward
[中文](README_zh.md)

/fləʊ-wərd/ (pronounced similar to Forward)

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
cmake --build build
```

## Sync Server Setup

This application requires a sync server to enable multi-device synchronization. You can deploy your own server using [ClipboardServer](https://github.com/L-Super/ClipboardServer).
### Docker Deployment
#### Quick Start with Docker Compose

1. Clone the ClipboardServer repository:
```bash
git clone https://github.com/L-Super/ClipboardServer.git
cd ClipboardServer
```

2. Start the server:
```bash
docker compose up -d --build
```

3. Access the web interface:
   - Open your browser and visit: `http://localhost:8000`
   - Register a new account or login
   - Configure the server URL in the application settings

### Manual Installation

1. Install dependencies:
```bash
# Using uv
uv sync
```

2. Start the server:
```bash
# Development environment
uvicorn main:app --host 0.0.0.0 --port 8000 --reload

# Production environment (background)
nohup uv run uvicorn main:app --host 0.0.0.0 --port 8000 &
```

3. Access the web interface at `http://localhost:8000`

### Environment Variables

Common environment variables (can also be set in `.env` file):

- `SECRET_KEY`: JWT secret key (change to a random value in production)
- `DATABASE_URL`: Database connection string (default: `sqlite:///data/clipboard.db`)
- `TZ`: Container timezone (e.g., `Asia/Shanghai`)

The service exposes port `8000` by default. If there's a port conflict, adjust the port mapping in `docker-compose.yml` (e.g., `18000:8000`).

### Configure the Application

After setting up the sync server, configure the application with:
- Server URL: `http://your-server-ip:8000` (or `https://your-domain.com` if using SSL)

## Contributing

Contributions welcome - submit issues and pull requests.

## License

[MIT License](LICENSE)