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

### 🎯 Key Components Summary

| Component                  | Role                                                                  |
| -------------------------- | --------------------------------------------------------------------- |
| **Global Trigger Manager** | Schedules jobs via cron/webhook; manages when ingestion should start. |
| **Ingestion Job Manager**  | Maintains registry of all jobs, their states, lifecycle control.      |
| **Ingestor**               | The core that executes `fetch → process → send`.                      |
| **Source Plugin**          | Modular fetchers for Git, Web, or Cloud data.                         |
| **Destination Plugin**     | Modular output handlers, like HTTP APIs.                              |

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
