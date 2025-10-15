# Changelog

All notable changes to the IDTF (Industrial Digital Twin Framework) project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [3.5.0] - 2025-10-15

### Added
- **IADL/FDL JSON Schema**: Formal JSON Schema definitions for IADL and FDL with conformance testing guidelines
- **Lifecycle Event Vocabulary**: Comprehensive lifecycle state definitions with event sources, transition rules, and write-back strategies
- **STRIDE Threat Model**: End-to-end threat analysis using STRIDE methodology
- **Reference Network Topology**: Purdue Model/ISA-95 aligned network segmentation with least privilege principles
- **ISA-95 Mapping Table**: Formal mapping between ISA-95 equipment hierarchy and IADL/FDL models
- **NDH Security & Governance Guide**: Implementation guide for RBAC/ABAC, audit logging, and data protection
- **Reference Connectors**: Specifications for OPC UA, ISA-95 Companion, MQTT Sparkplug B, and UNS connectors
- **Mono-repo Workspace**: NPM workspaces configuration for modular development
- **Docker Compose Environment**: Example deployment environment with all core services
- **CI/CD Pipeline**: GitHub Actions workflow for linting, schema validation, contract testing, and E2E testing
- **E2E Test Scripts**: Automated end-to-end testing framework
- **License Compliance**: Apache-2.0 license compliance checking in CI pipeline

### Changed
- **IDTF Overview**: Updated with multi-dimensional views, enhanced lifecycle management, and Cognite CDF-inspired features
- **IADL Editor Architecture**: Enhanced with richer data models, hierarchical refinement, and ISA-95 alignment
- **Factory Asset Library**: Improved database schema and API design to support extended attributes and lifecycle states
- **CAD/PLM Integration**: Enhanced data mapping to handle extended IADL properties
- **Asset Lifecycle Management**: Detailed lifecycle state definitions with formal event vocabulary
- **ERP/MES Connector**: Enhanced to handle extended business and production attributes
- **README**: Comprehensive update with architecture overview, quickstart guide, and solution blueprints

### Fixed
- Improved consistency across all architecture documents
- Standardized terminology and naming conventions
- Enhanced cross-reference integrity between documents

## [3.0.0] - 2024-XX-XX

### Added
- Initial IDTF V3.0 architecture
- IADL (Industrial Asset Definition Language) specification
- FDL (Factory Design Language) specification
- NDH (Neutral Data Hub) core architecture
- Basic connector agent framework
- IADL/FDL Editor initial design

---

## Release Notes

### Version 3.5.0 Highlights

This major release focuses on **production readiness**, **security**, and **ecosystem integration**:

1. **Formal Language Specifications**: IADL and FDL now have machine-readable JSON Schema definitions, enabling automated validation and tooling development.

2. **Enhanced Security**: Comprehensive STRIDE threat model and Purdue Model-aligned network topology provide enterprise-grade security guidance.

3. **Lifecycle Management**: Formal lifecycle event vocabulary ensures consistent asset state management across the entire ecosystem.

4. **Reference Implementations**: Pre-built connector specifications for major industrial protocols (OPC UA, MQTT Sparkplug B, UNS) accelerate integration.

5. **DevOps Ready**: Complete CI/CD pipeline, E2E testing framework, and containerized deployment examples enable rapid development and deployment.

6. **Standards Alignment**: Formal ISA-95 mapping table ensures interoperability with existing manufacturing systems.

### Breaking Changes

- Asset lifecycle states have been formalized. Existing implementations may need to update state names to match the new vocabulary.
- IADL/FDL JSON structure has been standardized. Legacy IADL/FDL files may require migration.

### Migration Guide

For users upgrading from IDTF V3.0 to V3.5:

1. Review the new IADL/FDL JSON Schema and validate existing asset definitions
2. Update asset lifecycle state names to match the formal vocabulary
3. Review security recommendations in the STRIDE threat model document
4. Consider implementing the reference network topology for production deployments

---

[3.5.0]: https://github.com/chchlin1018/IDTF-V3.5/releases/tag/v3.5.0
[3.0.0]: https://github.com/chchlin1018/IDTF-V3.5/releases/tag/v3.0.0

