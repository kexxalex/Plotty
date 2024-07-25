# Plotty

An open source CSV and Binary Data plotter, supporting differential geometry.

Supports plots as cartesian or spherical coordinates with default parameters / function if no data is present.<br>
Plot along tangent space of another Mesh which is interpolated by cubic splines.

### Example Screenshot
Sine curve along a geodesic of a sphere
![Sine curve along a geodesic of a sphere](https://github.com/kexxalex/Plotty/blob/main/Plotty.png)

## Clone
```bash
git clone --recursive https://github.com/kexxalex/Plotty.git
```
## Build (Linux)
```bash
cd Plotty
cmake . -B build -D CMAKE_BUILD_TYPE=Release
make -C build
```

## Build (Windows)
```bash
cd Plotty
cmake . -B build -D CMAKE_BUILD_TYPE=Release
```
Now open the the Plotty.sln in the build folder.

## Execution
It is necessary to run the application in the same folder where *"res"* is located.
