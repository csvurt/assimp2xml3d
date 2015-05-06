assimp2xml3d
========

### [XML3D](https://github.com/xml3d/xml3d.js) exporter leveraging the [Open Asset Import Library](https://github.com/assimp/assimp)
Modeled after Alexander Gessler's [assimp2json](https://github.com/acgessler/assimp2json)

### Output Format
This tool outputs the given model in XML3D's asset ```xml``` format, which can  be referenced from a ```<model>``` tag inside
an XML3D scene. The ID fragment of the asset will match the output file name without the extension:

```xml
<model src="my_model.xml#my_model"></model>
```

Each output file will contain a single ```<asset>```, nested ```<assets>``` are not supported.

### Build
assimp2xml3d can be built using CMake. Before building for the first time the ```assimp``` and ```tinyxml2``` submodules should be
updated using ```git submodule init && git submodule update```.

The resulting project files will be generated in a new ```/assimp2xml3d``` subdirectory while the executables will be copied to ```/bin``` by default.

### Usage

```bash
$ assimp2xml3d [-v|--verbose] input_file output_file
```

Both the input and output file names are required.
