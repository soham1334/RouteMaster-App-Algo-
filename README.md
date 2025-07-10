# Plugin-Based Data Ingester SDK – Detailed Architecture & Roadmap


## 🔰 Introduction

The **Plugin-Based Data Ingester SDK** is a modular and extensible framework for building and orchestrating data ingestion pipelines. Designed with a plugin-first architecture, it allows developers to pull data from various sources—such as **web pages**, **Git repositories**, and **cloud storage**—and push it into structured destinations like **APIs**. The SDK supports both **scheduled (cron)** and **event-based (webhook)** triggers, enabling flexible automation workflows.

This architecture document describes the system’s design, key components, technical constraints, and a week-by-week implementation roadmap. It is intended for **developers** building or extending plugins, **DevOps teams** managing deployment and integration, and **technical leads or product stakeholders** seeking a deeper understanding of the system’s structure and delivery plan.

---

## 1. ✅ High-Level Architecture

```
+---------------------------------------------------------------+
|                     Global Trigger Manager                    |
|    (Schedules & triggers ingestion via Cron/Webhook API)      |
+---------------------------------------------------------------+
                         |
                         ▼
+---------------------------------------------------------------+
|                    Ingestion Job Manager                      |
|  (Manages and monitors orchestrators for all ingestion jobs)  |
+---------------------------------------------------------------+
                         |
                         ▼
+-------------------------- Ingestor ---------------------------+
|           Orchestrates one job: source -> transform -> dest   |
+---------------------------------------------------------------+
           ▲                                    ▼
+----------------------+           +-----------------------------+
|     Source Plugin     |           |     Destination Plugin     |
| - Web, Git, Cloud     |           | - API Endpoint             |
| - Fetches data        |           | - Sends data               |
+----------------------+           +-----------------------------+
```

## 🎯 Key Components Summary

---

### 🔁 Global Trigger Manager

The **Global Trigger Manager** is responsible for starting ingestion jobs based on external triggers. These triggers can come in two main forms:

- **Cron Jobs:** Scheduled executions defined using cron expressions. For example, a user might configure a data ingestion task to run every hour or every day like  Monday at 9 AM.
- **Webhooks:** An HTTP API endpoint is exposed, and when it's called (usually by another system), it initiates the ingestion process.

This component ensures that jobs start at the right time or in response to specific external events. It also validates and prevents duplicate triggers for the same job when one is already running. Think of it as the gatekeeper that decides *when* to start ingestion.

---

### 🧠 Ingestion Job Manager

The **Ingestion Job Manager** maintains a registry of all ingestion jobs in the system. For each job, it keeps track of metadata like:

- The job's unique ID  
- Its current state (idle, running, completed, failed)  
- The associated source and destination configurations  
- Last run time, success/failure logs, etc.

It provides programmatic functions like starting or stopping a job, querying job status, and removing or updating jobs. This manager is essential for lifecycle control, enabling you to manage multiple ingestion jobs in a coordinated way. It also plays a key role in monitoring and debugging.

---

### ⚙️ Ingestor

The **Ingestor** is the core orchestration engine of the SDK. It takes a source plugin and a destination plugin and executes the ingestion pipeline:

1. Initializes and uses the source plugin to fetch the data  
2. Optionally applies a transformation or processing step  
3. Sends the fetched/processed data to the destination plugin

This component is designed to be lightweight, modular, and reusable. Each Ingestor instance handles a single run of a job, making it highly testable and predictable. It’s the "worker" that does the actual work of pulling and pushing data.

---

### 🌐 Source Plugin

A **Source Plugin** is a module that knows how to fetch data from a specific type of source. The SDK supports several types of source plugins:

- **Web Source Plugin:** Crawls web pages via sitemaps or recursive links  
- **Git Source Plugin:** Fetches data from version control systems like Git  
- **Cloud Storage Plugin:** Pulls files from providers like Google Drive or AWS S3

Each plugin implements a common interface to ensure consistency, regardless of data source. They encapsulate logic for authentication, discovery, and data retrieval, abstracting away the complexity from the core system.

---

### 📤 Destination Plugin

The **Destination Plugin** is responsible for sending data to a configured target endpoint. Initially, the SDK will support:

- A **generic API endpoint** that accepts HTTP POST requests  
- **Authentication mechanisms**, such as tokens or API keys  
- **Batching** capabilities to improve throughput and reduce network overhead

Destination plugins follow a uniform interface, making them easy to replace or extend. In the future, additional destinations like databases, cloud functions, or message queues can be supported.

---


## 2. 🧰 Low-Level Architecture

### 📁 SDK Project Structure

```text
plugin-ingester-sdk/
├── src/
│   ├── core/
│   │   ├── GlobalTriggerManager.js
│   │   ├── IngestionJobManager.js
│   │   └── Ingestor.js
│   ├── interfaces/
│   │   ├── SourcePlugin.js
│   │   └── DestinationPlugin.js
│   ├── plugins/
│   │   ├── sources/
│   │   │   ├── WebSourcePlugin.js
│   │   │   ├── GitSourcePlugin.js
│   │   │   └── CloudStoragePlugin.js
│   │   └── destinations/
│   │       └── ApiDestinationPlugin.js
│   ├── utils/
│   │   ├── logger.js
│   │   └── scheduler.js
│   └── index.js
├── examples/
├── docs/
└── test/
```

---

## 📌 Component-Level Details

---

### 📌 `GlobalTriggerManager.js`

This file is responsible for managing **how and when** ingestion jobs are triggered. It supports two main mechanisms:

- **Cron Scheduling**: Uses libraries like `node-cron` to schedule ingestion jobs periodically based on cron expressions. For example, a job can be configured to run every hour or once a day.
  
- **Webhooks**: Exposes HTTP endpoints via Express, allowing external systems to trigger ingestion jobs on-demand (e.g., when a GitHub push event occurs).

The main responsibilities of this component include:

- Registering new ingestion jobs with associated triggers.
- Starting or stopping jobs based on schedule or webhook events.
- Preventing duplicate runs by checking if a job is already running.
- Retrying or re-triggering jobs by ID.

This acts as the centralized event entry point for the ingestion lifecycle.

---

### 📌 `IngestionJobManager.js`

This acts as the **central job controller** and registry that keeps track of every configured ingestion job.

Key responsibilities:

- Maintain a list of all active and scheduled jobs.
- Track job metadata including:
  - Unique job ID
  - Associated source and destination configurations
  - Last execution time
  - Current job status (idle, running, completed, failed)
  
- Provide an API-like interface to manage jobs programmatically:

```js
registerJob(jobConfig)     // Adds a new job
startJob(jobId)            // Starts execution of a job
stopJob(jobId)             // Stops a running job
getJobStatus(jobId)        // Retrieves status and metadata of a job
```

This module ensures that jobs can be dynamically added, removed, or monitored at runtime.

---

### 📌 `Ingestor.js`

The `Ingestor` is the **core engine** that executes the data pipeline for a single job. It takes two plugin instances: one source and one destination, and performs the following sequence:

1. Calls `sourcePlugin.initialize()` — prepares connection/authentication.
2. Calls `sourcePlugin.fetchData()` — fetches raw data from the configured source.
3. Optionally transforms, filters, or enriches the data (can be injected later).
4. Calls `destinationPlugin.sendData(data)` — sends the result to the output destination.

The `Ingestor` is designed to be lightweight and job-specific. It can later be extended to support:

- Retry logic
- Transformation pipelines
- Real-time feedback and progress tracking

---

### 📌 `SourcePlugin.js` Interface

This is a base interface that all source plugins (Git, Web, Cloud, etc.) must implement. It defines the contract for how the SDK expects to interact with any data source.

```js
class SourcePlugin {
  constructor(config) {}

  async initialize() {
    // Optional: setup credentials, validate connection
  }

  async fetchData() {
    throw new Error("Not Implemented");
  }

  async getStatus() {
    // Optional: returns health/status info
  }
}
```

Each source plugin must at least implement `fetchData()` to return raw content. `initialize()` can be used to set up API tokens or auth headers.

---

### 📌 `DestinationPlugin.js` Interface

This is the standard contract that all output/destination plugins must follow. It enables consistent sending of data to any destination like an API, database, or message queue.

```js
class DestinationPlugin {
  constructor(config) {}

  async initialize() {
    // Optional: setup connection or auth
  }

  async sendData(data) {
    throw new Error("Not Implemented");
  }

  async sendBatch(dataArray) {
    // Optional: optimized batch sending
  }

  async getStatus() {
    // Optional: reports connectivity or send stats
  }
}
```

- `sendData(data)` is required — handles single data unit transmission.
- `sendBatch(dataArray)` is optional but useful for performance.
- `getStatus()` can help with monitoring or debugging.

---
---

### 📂 `plugins/`

This directory houses all the **source** and **destination plugin implementations**. These are concrete classes that extend the base interfaces (`SourcePlugin.js` and `DestinationPlugin.js`) and implement logic for specific systems like Git, Web, Cloud Storage, and HTTP APIs.

---

### 📌 `plugins/sources/WebSourcePlugin.js`

The `WebSourcePlugin` is a plugin that fetches data from websites.

#### Responsibilities:
- Crawl content from a given URL.
- Discover additional links via:
  - Sitemap parsing
  - Recursive link crawling (up to a specified depth)
- Extract HTML content or structured data (optional transformation).
- De-duplicate and normalize URLs.

#### Expected Config:
```js
{
  startUrl: "https://example.com",
  crawlDepth: 2,
  includeSitemaps: true
}
```

Useful for ingesting open web data like blogs, product listings, or documentation pages.

---

### 📌 `plugins/sources/GitSourcePlugin.js`

The `GitSourcePlugin` pulls files from a Git repository.

#### Responsibilities:
- Clone or pull from a Git URL (public or private).
- Checkout to the specified branch.
- Extract file content (optionally filter by file type, path, etc.).
- Handle authentication via SSH or HTTPS.

#### Expected Config:
```js
{
  repoUrl: "https://github.com/user/repo.git",
  branch: "main",
  includePaths: ["src/", "docs/"]
}
```

This plugin is useful for ingesting codebases, documentation, or markdown-based content.

---

### 📌 `plugins/sources/CloudStoragePlugin.js`

This plugin integrates with cloud storage providers like Google Drive, AWS S3, or Azure Blob.

#### Responsibilities:
- Authenticate with the provider using OAuth, access keys, or service accounts.
- List and read files from a specific folder/bucket/path.
- Filter by file type or modification date.

#### Expected Config:
```js
{
  provider: "gdrive", // or "s3", "azure"
  path: "/folder-name/",
  auth: {
    apiKey: "xxx", // or OAuth credentials
  }
}
```

Useful for syncing backups, exported reports, or shared content.

---

### 📌 `plugins/destinations/ApiDestinationPlugin.js`

This destination plugin pushes data to a generic HTTP API endpoint.

#### Responsibilities:
- POST data to a specified URL.
- Handle auth via headers, tokens, or API keys.
- Support for both:
  - `sendData()` — single item
  - `sendBatch()` — array of items
- Handle retries, timeouts, and logging failures.

#### Expected Config:
```js
{
  endpoint: "https://api.example.com/ingest",
  headers: {
    Authorization: "Bearer token123"
  },
  batchSize: 10
}
```

Ideal for pushing data into indexing systems, databases, dashboards, or downstream processing pipelines.

---

### 📂 `utils/`

The `utils/` directory contains utility functions and shared modules used across the SDK.

---

### 📌 `utils/logger.js`

Provides logging functionality for the SDK.

#### Features:
- Custom logging levels: info, debug, error, warn.
- Option to write logs to console or a file.
- Tagging and timestamps for each message.
- Can be enhanced to integrate with log monitoring tools (like Winston, Pino, or Sentry).

Used across core and plugin modules for consistent and structured logging.

---

### 📌 `utils/scheduler.js`

Handles time-based scheduling using cron.

#### Features:
- Wrapper around `node-cron`.
- Can register, start, stop, and update cron jobs by ID.
- Emits events when jobs are triggered.
- Can be reused by the `GlobalTriggerManager`.

This abstracts away raw cron handling and makes scheduling ingestion tasks programmatically easier.

---

### 📌 `index.js`

This is the SDK’s entry point.

#### Responsibilities:
- Export main classes like `Ingestor`, `IngestionJobManager`, and plugin base interfaces.
- Optionally, expose utility functions or helpers.
- Acts as the public API when the SDK is consumed via `require()` or `import`.

Example usage in a consumer project:

```js
const { Ingestor, SourcePlugin, DestinationPlugin } = require("plugin-ingester-sdk");
```

---

## 3. 🗓️ Implementation Roadmap (3 Weeks)

---

### 🔹 Week 1: Foundation & Orchestrator Setup

**Objective:** Set up the SDK structure, define interfaces, and implement the ingestion core.

1. **Project Bootstrapping**  
   - Initialize the project with `npm init` or `pnpm init`.
   - Set up the project folder structure (as per the architecture).
   - Configure ESLint and Prettier for code formatting.
   - Add Jest (or Vitest) for testing.
   - Setup Babel or TypeScript if needed for modern syntax.

2. **Define Core Interfaces**  
   - Implement abstract classes/interfaces for `SourcePlugin` and `DestinationPlugin`.
   - Define method contracts like `initialize()`, `fetchData()`, `sendData()`, and `getStatus()`.

3. **Develop Ingestor Engine**  
   - Implement the `Ingestor.js` class.
   - Accept source and destination plugin instances and execute the ingestion flow:
     1. Source: Initialize → Fetch
     2. Optional transformation placeholder
     3. Destination: Send data or send batch

4. **Logging Utility**  
   - Create a simple `logger.js` using `Winston` or `Pino`.
   - Export functions like `logInfo()`, `logError()`, `logDebug()` to standardize logs across modules.

5. **Initial Unit Testing**  
   - Write basic test cases for:
     - Interface enforcement
     - A mocked run of the Ingestor using dummy plugins
   - Ensure Jest runs correctly with setup and teardown configured.

---

### 🔹 Week 2: Plugin Implementation & Triggering Core

**Objective:** Build essential plugins and the orchestration layer to support scheduled and on-demand ingestion.

1. **Web Source Plugin**  
   - Build the plugin to support:
     - Basic sitemap parsing using `sitemap-parser` or regex
     - Recursive crawling with `axios` and `cheerio`
   - Filter and return clean page content or URLs.

2. **Git Source Plugin**  
   - Use `simple-git` to clone a repo to a temp directory.
   - Read files under specified paths (e.g., `src/`, `docs/`).
   - Return file content with metadata.

3. **Cloud Storage Plugin (Google Drive / S3)**  
   - For Google Drive:
     - Use OAuth2 via `googleapis` package.
     - List and download files from a specific folder.
   - For AWS S3:
     - Use `@aws-sdk/client-s3` to list objects and download files.
     - Handle auth via IAM or access key.

4. **API Destination Plugin**  
   - Accept data (single or batch) and POST to a provided endpoint.
   - Support:
     - Auth headers (e.g., bearer tokens)
     - Batch sending with configurable size
     - Retry on failure (basic implementation)

5. **Global Trigger Manager**  
   - Use `node-cron` to schedule jobs based on cron expressions.
   - Set up a minimal Express server to expose webhook endpoints.
   - On cron or webhook hit, trigger the corresponding job via the Job Manager.

6. **Ingestion Job Manager**  
   - Maintain job metadata in-memory (for now).
   - Implement functions to:
     - Register a job
     - Start/Stop jobs by ID
     - Query job status and last run info

---

### 🔹 Week 3: Integrations, CLI, Testing & Docs

**Objective:** Add polish to the SDK: integration logic, examples, docs, packaging.

1. **Plugin Loader System**  
   - Implement logic to load plugin instances dynamically from a config object.
   - Allow easy extension of new plugins without modifying Ingestor core.

2. **Minimal CLI or Local API Server**  
   - Build a basic CLI (`index.js` or using `commander`) to test plugins and ingestion.
   - Optional: Serve a minimal Express-based API for remote job control.

3. **End-to-End Testing**  
   - Simulate complete ingestion:
     - Web → API
     - Git → API
   - Use mocks or local test servers for destination endpoints.
   - Validate all error paths, retries, logging, and job status.

4. **Write Documentation**  
   - Create a README with setup, usage, and architecture.
   - Add:
     - Getting Started guide
     - API Reference
     - Plugin Development Guide
   - Optional: Host docs using GitHub Pages or Docusaurus.

5. **Publish SDK Package**  
   - Prepare `package.json` with proper `main` and `types` entries.
   - Publish to npm (public or scoped private).
   - Tag release and push to GitHub.

6. **(Optional)**: Add Hook for Data Transformation  
   - Add a method or middleware point in `Ingestor.js` to modify or enrich data between fetch and send.

---

## 4. 📦 Final Deliverables

- ✅ An installable NPM SDK package.
- ✅ Source + destination plugin implementations.
- ✅ Fully working ingestion core (Ingestor).
- ✅ CLI tool or test runner to demonstrate ingestion.
- ✅ README and plugin developer documentation.
- ✅ 80%+ test coverage for core logic.
- ✅ Template for creating custom plugins.
