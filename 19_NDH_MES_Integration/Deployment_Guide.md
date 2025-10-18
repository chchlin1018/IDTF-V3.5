

# NDH-MES 整合部署指南

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件提供了 NDH-MES 整合的詳細部署指南,包括模組結構、部署方式和配置管理。

## 2. 模組結構

```
ndh/
├── api/
├── core/
├── mes/
│   ├── __init__.py
│   ├── main.py
│   ├── api.py
│   ├── dispatcher.py
│   ├── manager.py
│   └── adapters/
│       ├── __init__.py
│       ├── base.py
│       ├── aveva.py
│       └── factorytalk.py
└── ...
```

## 3. 部署方式

### 3.1 Docker Compose

```yaml
version: "3.8"

services:
  ndh:
    build: .
    ports:
      - "8000:8000"
    volumes:
      - ./config:/app/config

  rabbitmq:
    image: "rabbitmq:3-management"
    ports:
      - "5672:5672"
      - "15672:15672"
```

### 3.2 Kubernetes

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: ndh
spec:
  replicas: 3
  selector:
    matchLabels:
      app: ndh
  template:
    metadata:
      labels:
        app: ndh
    spec:
      containers:
        - name: ndh
          image: my-ndh-image
          ports:
            - containerPort: 8000
          volumeMounts:
            - name: config-volume
              mountPath: /app/config
      volumes:
        - name: config-volume
          configMap:
            name: ndh-config
```

---

## 4. 配置管理

### 4.1 config.yaml

```yaml
mes:
  enabled: true
  adapters:
    - name: "aveva"
      enabled: true
      config:
        api_url: "http://aveva/api"
        event_broker_url: "amqp://guest:guest@rabbitmq:5672/"
    - name: "factorytalk"
      enabled: true
      config:
        api_url: "http://factorytalk/api"
        opc_ua_url: "opc.tcp://factorytalk:4840"
```

### 4.2 適配器管理器 (Adapter Manager)

```python
import yaml

class AdapterManager:
    def __init__(self, config_path):
        with open(config_path, "r") as f:
            self.config = yaml.safe_load(f)
        self.adapters = {}

    def load_adapters(self):
        for adapter_config in self.config["mes"]["adapters"]:
            if adapter_config["enabled"]:
                adapter_name = adapter_config["name"]
                adapter_class = self.get_adapter_class(adapter_name)
                self.adapters[adapter_name] = adapter_class(adapter_config["config"])

    def get_adapter_class(self, name):
        if name == "aveva":
            from .adapters.aveva import AvevaMESAdapter
            return AvevaMESAdapter
        elif name == "factorytalk":
            from .adapters.factorytalk import FactoryTalkMESAdapter
            return FactoryTalkMESAdapter
```

---

## 5. 預期結果

- **無縫整合**: MES 虛擬介面層作為 NDH 的一個核心模組,實現無縫整合。
- **可配置性**: 支援透過 NDH 的配置檔案來配置 MES 虛擬介面層。
- **統一管理**: 透過 NDH 的管理介面來管理 MES 虛擬介面層。

---

