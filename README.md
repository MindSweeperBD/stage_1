# Stage 1 - Building the Data Layer
The repository contains the implementation developed for the Stage 1 of the Big Data project 26/27 at the University of Las Palmas de Gran Canaria.

The goal of the project is to build a search engine from zero. This first stage focuses on designing and implementing the data layer, which will prepares and organizes all the books that we will use on the next stages . The dataset used thoughout the project is from **Project Gutenberg**, a digital library of public-domain books.

## Objectives
The main objective of the stage is to build a data pipeline capable of downloading, processing, indexing and storing book data. Our data layer is made up by three main components:
- The **Datalake** where we store the downloaded and processed content.
- The **Datamart** that stores structured metadata and inverted indexes, which are used for efficient searches.
- And the **Control Layer** who coordinates the different stages of the pipeline and keeps track of the processing state.

For an additional objective we evaluate different implementations and storages through benchmarking. It includes implementations in **Python**, **Java** and **C++**, that we will compare to each other.

## Implementations
The project implements 3 different programming languages which follows the same general pipeline and are used as part of the performance comparation:
- **Pyhton**, whose main components are a *Downloader*, a *Datamart*, a *Control Layer* and it's own *Benchmarks*.
- **Java**, as Python, it has a Downloader, a Control Layer and it's benchmarks, but, instead of a Datamart, it has an *Indexer*.
- **C++**, it's main components are the same as the Python components, *Downloader*, *Datamart*, *Control Layer* and *Benchmarks*.

## Repository Structure
Each language contains the components required for implement the pipeline and perform the benchmarks.
```
stage_1/ 
│
├── C++/
│     ├── Downloader/
│     ├── BusinessUnit/
│     └── Control/
│
├── Java/
│     ├── Downloader/
│     ├── Indexer/
│     └── Control/
│
├── Python/
│     ├── Downloader/
│     ├── Index/
│     ├── Controller/
│     └── Benchmark/
│
└── README.md
```

## Requirements
In order to use the project it important to have on your computer ***Python 3***, ***request*** and ***pymongo*** for the Python section. 

In other hand for Java, it is necessary have ***Java 21*** and ***Maven***. 

At last but no less important, C++ requires ***C++20 compatible compiler***, ***CMake 3.16*** or later, ***SQLite3***, ***ICU***, ***libcurl*** and ***MongoDB C++ drivers*** for the MongoDB implementation.
