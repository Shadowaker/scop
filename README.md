# Scop

## 📖 Overview

**Scop** is a project from 42 school that consists of implementing a 3D object viewer.
The goal is to load and display `.obj` files, apply 3D transformations, and allow interaction with the model through keyboard and mouse input.

## 🚀 Features

* Parsing of any `.obj` file (vertices, faces, normals, UV).
* 3D rendering using **OpenGL**.
* Support for transformations:

  * Translation
  * Rotation
  * Scaling
* Perspective projection and camera management.
* Keyboard/mouse controls to manipulate the model.
* Possible extensions: shading, texture mapping, materials.

## 🛠️ Requirements

* **C++ compiler** (c++17)
* **Makefile**
* **OpenGL 3+**
* **GLFW**
* **GLEW**

On Ubuntu/Debian you can install dependencies with:

```bash
sudo apt-get install cmake build-essential libglfw3-dev libglew-dev
```

## ⚙️ Installation & Usage

Clone the repository and compile:

```bash
git clone https://github.com/<your-username>/scop.git
cd scop
make
```

Run the program passing a `.obj` model:

```bash
./scop path/to/model.obj
```

## 🎮 Controls

* **W/A/S/D**: move the camera
* **↑/↓/←/→**: rotate the camera
* **T**: cycle through provided textures
* **L**: increase or decrease light intensity
* **Shift**: switch between different modes
* **ESC**: quit

## 📸 Preview

