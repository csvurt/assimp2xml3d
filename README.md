assimp2xml3d
========

### [XML3D](https://github.com/xml3d/xml3d.js) exporter leveraging the [Open Asset Import Library](https://github.com/assimp/assimp)
Modeled after Alexander Gessler's [assimp2json](https://github.com/acgessler/assimp2json)

This command line tool converts various 3D formats to XML3D's asset format using Assimp. See [Assimp's website](http://assimp.sourceforge.net/main_features_formats.html) 
for a complete list of supported input formats.

Animated models are currently not supported but are planned for a future version.

### Output Format
This tool outputs the given model in XML3D's asset ```xml``` format, which can  be referenced from a ```<model>``` tag inside
an XML3D scene. The ID fragment of the asset will match the output file name without the extension:

```xml
<model src="my_model.xml#my_model"></model>
```

Each output file will contain a single ```<asset>```, nested ```<assets>``` are not supported.

### Build
assimp2xml3d is built using CMake. Before building for the first time the ```assimp``` and ```tinyxml2``` submodules should be
updated using ```git submodule init && git submodule update```.

It's recommended that you target an x64 architecture since Assimp can consume a lot of memory when processing large models. 

### Usage

```bash
$ assimp2xml3d [--verbose|--no-materials] input_file output_file
```

Both the input and output file names are required. The tool will output warnings and errors by default, more info output can be toggled
with the ```-v``` or ```--verbose``` flags. 

Use the ```--no-materials``` or ```-m``` flag to export the model without materials. Even if the input file has no materials the tool will generate a 
default material unless this flag is set. 

### Changelog

10.06.2016 - v0.2.1

* Updated Assimp and tinyxml2 dependencies to latest master builds
* Ignore path prefixes when determining a name for the exported asset
* Fixed a case where material ID references would not be sanitized for HTML

20.10.2015 - v0.2.0

* Move to XML3D 5.0, XML3D 4.x is no longer supported
* Added a --no-materials (-m) flag
* Disabled mesh splitting since XML3D now uses the [OES_element_index_uint](https://www.khronos.org/registry/webgl/extensions/OES_element_index_uint/) extension

28.07.2015 - v0.1.1

* Fixed issues: [#2](https://github.com/csvurt/assimp2xml3d/issues/2)

12.05.2015 - v0.1.0

* Initial release
* Support for meshes and textured materials
* Mesh splitting
