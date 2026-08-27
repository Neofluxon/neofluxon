# Neofluxon

Neofluxon is intended to become a photography workflow application
that combines Digital Asset Management (DAM) with non-destructive photo editing.

## Development Goals

The first development goal is the DAM foundation: providing a focused solution for managing photography assets.

The main planned features include:

- Browse the filesystem and import photos as new shoots/sessions,
  supporting a linear workflow (culling, rating, organization, etc.).

- Low-latency browsing and previews from both the filesystem and the library,
  with an initial performance target of maintaining smooth interaction
  with collections containing up to 100,000 images.

- A library with multiple representations of the same assets:
  - Folder tree.
  - Date/time tree.
  - Equipment tree (camera and lenses).
  - Keywords tree.
  - Collections.
  - Other representations.

- Support for multiple libraries.

- Metadata management:
  ratings, keywords, captions, copyright, GPS.

- Search capabilities and saved searches.

- File tracking and duplicate detection.

- File integrity checking.

- Batch operations for managing multiple photos
  (metadata, ratings, keywords, organization, etc.).

The screenshot below shows the current state of the application.
Work in progress.

![Current Neofluxon view](packages/screenshot.png)

### Current Development Status

The project is currently under early development, and no official release is available yet.

The following features have been partially implemented and are currently available for testing purposes only:

- Filesystem browsing for RAW photos.

- Display of thumbnails and previews (grid layout or filmstrip with preview).

- Keyboard navigation for previewing images.

- Supported image formats:
  - Canon (.cr2, .cr3).
  - Nikon (.nef).
  - Fujifilm (.raf).
  - Adobe Digital Negative (.dng).
  - JPEG (.jpg), PNG (.png).

- Some library features:
  - Adding folders to the library.
  - Navigating the library.

### Technical Overview

Neofluxon is developed in **C++** and **Qt**. The codebase is architecturally divided into two primary layers:

* **Core:** Written in pure C++, maintaining zero dependency on the GUI framework.
* **UI:** The graphical interface layer implemented using Qt (C++).

### Free Software

Neofluxon is [Free Software](https://www.gnu.org/philosophy/free-sw.en.html), released under the **GPLv3** license. The term "free" refers to freedom—users are free to run, study, modify, and distribute the software. It does not refer to price.

### Dependencies

* **Qt6**
* **LibRaw** ([https://www.libraw.org](https://www.libraw.org))
* **libjpeg-turbo** (https://libjpeg-turbo.org/)
* **stb** (https://github.com/nothings/stb)
* **Zlib** (https://zlib.net/)
* **libpng** (https://www.libpng.org/pub/png/libpng.html)

### Build Requirements

* **CMake** ≥ 3.25
* **C++26**
