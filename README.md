# Neofluxon

Here I am trying to develop a simple photography workflow software, first for myself,
as a tool that fits my needs and is not intended for advanced users.

A little philosophy behind this may change or improve over time; these are the central ideas:

- Filesystem is the source of truth. The main organization is the filesystem.
- Files outlive software. Photos, edits, and exports should remain accessible independently of the application.
- Workflow defines organization. Sessions group originals, edits, and exports as they are produced.
- Non-destructive. Originals are never modified.
- Data assets, organization, and edits remain accessible. They stay usable with standard tools, without dependency on the software.

Every import is defined as a Session/Shoot.

A shoot/session will generally represent a standard linear workflow:
import → culling → edit → export → archive

According to the third law of thermodynamics, entropy always increases.
No matter how organized a user might be, chaos eventually wins.
The plan is for the software to enforce/automate a filesystem structure for data assets along the workflow steps.

The first stage focuses on a single part of the workflow: organization of digital assets:
import → culling → archive

The second stage will be non-destructive editing and export.

### Current Status

The project is in development/experimental stage.
No official releases are available yet.
 See repository tags for current development snapshots.

### What it can do now

- File system browsing for RAW photos
- Display thumbnails and previews (grid layout or filmstrip + preview)
- Keyboard navigation for previewing images
- Supported image fromats:
    * Canon (`.cr2`, `.cr3`)
    * Nikon (`.nef`)
    * Fujifilm (`.raf`)
    * Adobe Digital Negative (`.dng`)
    * `JPG`, `PNG`

### Technical Overview & Licensing

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
* **C++23**
