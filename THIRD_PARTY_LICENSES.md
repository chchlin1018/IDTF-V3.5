# Third-Party Licenses

This document lists all third-party dependencies used in the IDTF (Industrial Digital Twin Framework) project and their respective licenses. All dependencies are compatible with the Apache-2.0 license under which IDTF is distributed.

## Runtime Dependencies

### Core Framework

| Package | Version | License | Purpose |
|:---|:---|:---|:---|
| PostgreSQL | 15.x | PostgreSQL License (similar to MIT) | Primary database for NDH |
| Redis | 7.x | BSD-3-Clause | Caching and message broker |
| gRPC | 1.x | Apache-2.0 | Inter-service communication |
| Protocol Buffers | 3.x | BSD-3-Clause | Data serialization |

### Connector Agents

| Package | Version | License | Purpose |
|:---|:---|:---|:---|
| node-opcua | 2.x | MIT | OPC UA client library |
| mqtt.js | 5.x | MIT | MQTT client library |
| axios | 1.x | MIT | HTTP client |

### Web Applications

| Package | Version | License | Purpose |
|:---|:---|:---|:---|
| React | 18.x | MIT | UI framework for IADL Editor |
| Next.js | 14.x | MIT | React framework |
| Tailwind CSS | 3.x | MIT | CSS framework |
| shadcn/ui | latest | MIT | UI component library |

## Development Dependencies

| Package | Version | License | Purpose |
|:---|:---|:---|:---|
| ESLint | 8.x | MIT | Code linting |
| Jest | 29.x | MIT | Testing framework |
| TypeScript | 5.x | Apache-2.0 | Type checking |
| Prettier | 3.x | MIT | Code formatting |
| Ajv | 8.x | MIT | JSON Schema validation |

## Container Images

| Image | Version | License | Purpose |
|:---|:---|:---|:---|
| postgres | 15-alpine | PostgreSQL License | Database |
| redis | 7-alpine | BSD-3-Clause | Cache |
| eclipse-mosquitto | 2 | EPL-2.0 / EDL-1.0 | MQTT broker |
| node | 18-alpine | MIT | Runtime environment |

## Standards and Specifications

| Standard | License/Terms | Usage |
|:---|:---|:---|
| OPC UA | OPC Foundation License | Protocol implementation |
| ISA-95 | ISA License | Reference model alignment |
| MQTT Sparkplug B | Eclipse Public License 2.0 | Protocol specification |
| JSON Schema | BSD-3-Clause | Schema definition |

## License Compatibility

All third-party dependencies have been carefully selected to ensure compatibility with the Apache-2.0 license. The following licenses are considered compatible:

- **Apache-2.0**: Fully compatible
- **MIT**: Fully compatible
- **BSD-2-Clause / BSD-3-Clause**: Fully compatible
- **ISC**: Fully compatible
- **PostgreSQL License**: Fully compatible (similar to MIT)
- **EPL-2.0 / EDL-1.0**: Compatible with proper attribution

## Compliance

This project uses automated license checking in the CI/CD pipeline to ensure ongoing compliance. The license checker is configured to only allow the following licenses:

```
Apache-2.0, MIT, BSD-2-Clause, BSD-3-Clause, ISC
```

Any dependency with a non-compatible license will cause the build to fail.

## Attribution

Full license texts for all dependencies can be found in the `node_modules` directory after installation, or in the respective project repositories.

## Updates

This file is automatically updated as part of the release process. For the most current dependency information, run:

```bash
npx license-checker --production --json > licenses.json
```

---

**Last Updated**: 2025-10-15
**IDTF Version**: 3.5.0

