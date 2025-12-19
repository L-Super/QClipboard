# Floward

/fləʊ-wərd/ （近似 Forward）

一个基于 Qt 的跨平台剪贴板管理工具，支持多设备同步。可以从剪贴板历史记录中选择对应的项进行粘贴。它提供了一种查看剪贴板历史记录和快速粘贴任何以前复制的内容的简便方式。

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
cmake --build build
```

## 同步服务配置

本应用需要同步服务器才能实现多设备同步功能。您可以使用 [ClipboardServer](https://github.com/L-Super/ClipboardServer) 部署自己的服务器。
### Docker 部署
#### 使用 Docker Compose 快速开始

1. 克隆 ClipboardServer 仓库：
```bash
git clone https://github.com/L-Super/ClipboardServer.git
cd ClipboardServer
```

2. 启动服务器：
```bash
docker compose up -d --build
```

3. 访问 Web 界面：
   - 打开浏览器访问：`http://localhost:8000`
   - 注册新账户或登录
   - 在应用设置中配置服务器地址

### 手动安装

1. 安装依赖：
```bash
# 使用 uv
uv sync
```

2. 启动服务器：
```bash
# 开发环境
uvicorn main:app --host 0.0.0.0 --port 8000 --reload

# 生产环境（后台运行）
nohup uv run uvicorn main:app --host 0.0.0.0 --port 8000 &
```

3. 访问 Web 界面：`http://localhost:8000`

### 环境变量

常用环境变量（也可在 `.env` 文件中设置）：

- `SECRET_KEY`：JWT 密钥（生产环境请改为随机值）
- `DATABASE_URL`：数据库连接字符串（默认：`sqlite:///data/clipboard.db`）
- `TZ`：容器时区（例如：`Asia/Shanghai`）

服务默认暴露端口 `8000`。如果端口冲突，可在 `docker-compose.yml` 中调整端口映射（例如：`18000:8000`）。


### 配置应用

设置好同步服务器后，在应用中配置：
- 服务器地址：`http://your-server-ip:8000`（如果使用 SSL，则为 `https://your-domain.com`）

## 贡献

欢迎贡献 - 提交 issue 和 PR。

## 许可证

[MIT License](LICENSE)