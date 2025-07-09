# Plugin-Based Data Ingester SDK – Detailed Architecture & Roadmap

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

- **Cron Jobs:** Scheduled executions defined using cron expressions. For example, a user might configure a data ingestion task to run every hour or every Monday at 9 AM.
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

### 📌 Component-Level Details

---

### 📌 `GlobalTriggerManager.js`

* Uses cron expressions via `node-cron`.
* Also exposes webhook handlers using `express` for external API triggering.
* Manages:

  * Registering a job
  * Starting/stopping a job
  * Re-triggering by ID
  * Validating if a job is already running

---

### 📌 `IngestionJobManager.js`

* Acts like a **Job Registry & Controller**.
* Maintains:

  * List of active jobs
  * Job status (idle, running, failed, completed)
  * Metadata (source, destination configs, last run time)
* APIs:

  ```js
  registerJob(jobConfig)
  startJob(jobId)
  stopJob(jobId)
  getJobStatus(jobId)
  ```

---

### 📌 `Ingestor.js`

* Core runner logic. Coordinates:

  1. `sourcePlugin.initialize()`
  2. `sourcePlugin.fetchData()`
  3. Optional processing logic
  4. `destinationPlugin.sendData(data)`
* Can be extended to support data transformation or filtering between steps.

---

### 📌 `SourcePlugin.js` Interface

```js
class SourcePlugin {
  constructor(config) {}
  async initialize() {}
  async fetchData() { throw new Error("Not Implemented"); }
  async getStatus() {}
}
```

### 📌 `DestinationPlugin.js` Interface

```js
class DestinationPlugin {
  constructor(config) {}
  async initialize() {}
  async sendData(data) { throw new Error("Not Implemented"); }
  async sendBatch(dataArray) {}
  async getStatus() {}
}
```

---

## 3. 🿓️ Implementation Roadmap (3 Weeks)

---

### 🔹 **Week 1: Foundation & Orchestrator**

| Task                    | Details                                                        |
| ----------------------- | -------------------------------------------------------------- |
| ✅ Project Bootstrapping | Setup folder structure, linter, test config                    |
| ✅ Define Interfaces     | `SourcePlugin`, `DestinationPlugin`                            |
| ✅ Ingestor Class        | Write `Ingestor.js` to run ingestion between a pair of plugins |
| ✅ Logging Utility       | Setup Winston or Pino                                          |
| ✅ Initial Unit Tests    | Add basic Jest test cases for interfaces & Ingestor            |

---

### 🔹 **Week 2: Core Plugin Implementation & Triggering**

| Task                     | Details                                                 |
| ------------------------ | ------------------------------------------------------- |
| ✅ Web Source Plugin      | Uses sitemap or recursive crawling (`cheerio`, `axios`) |
| ✅ Git Source Plugin      | Uses `simple-git` to clone/read files                   |
| ✅ Cloud Storage Plugin   | GDrive (OAuth), S3 (AWS SDK)                            |
| ✅ API Destination Plugin | Supports batching, auth headers                         |
| ✅ GlobalTriggerManager   | Cron + Webhook-based triggering                         |
| ✅ Job Manager            | Manages job lifecycle (register/start/stop/status)      |

---

### 🔹 **Week 3: Integrations, Docs & Testing**

| Task                                                 | Details                                        |
| ---------------------------------------------------- | ---------------------------------------------- |
| ✅ Plugin Loader                                      | Dynamic plugin registration via config         |
| ✅ Write CLI or Minimal API Server                    | For testing and examples                       |
| ✅ End-to-End Tests                                   | Simulate ingestions from source to destination |
| ✅ Docs Writing                                       | README, API Reference, Plugin Dev Guide        |
| ✅ Publish Package                                    | `npm publish`, scoped private/public           |
| ✅ Optional: Add data transformation hook in Ingestor |                                                |

---

## 4. 📦 Final Deliverables

| Deliverable                  | Description                                  |
| ---------------------------- | -------------------------------------------- |
| **NPM Package**              | Installable SDK via npm                      |
| **Documentation Site**       | Hosted on GitHub Pages / Docusaurus          |
| **Example Scripts**          | Git-to-API, Web-to-API etc.                  |
| **Unit + Integration Tests** | > 80% test coverage                          |
| **Plugin Dev Template**      | Starter boilerplate to create custom plugins |
