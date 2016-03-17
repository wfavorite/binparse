# binparse - A Binary parsing tool

A tool for reading binary file description files (BPF - Bin Parse Format), and
using these to reference fields and data types in a binary file.

How it works - The user view
----------------------------

A BPF file can contain one or more of the following lines:
 - A parse point - How to interpret and display a data location in a binary file.
 - An enum definition - A *mapping* between a value and a string for decoding fields into meaningful values.
 - An explicit tag - Used to define a frequently used location in the binary file.
 - An option - Provides the ability to "pass" command line options in the BPF file. 
 - Comments or blank lines

### Parse point

A parse point is a line that describes a piece of data in the file, how to interpret it, and assigns a tag to that data. The following items are required for the parse point:
 - Offset (in bytes, from the start of the file)
 - Size (in bytes, of the object to be read)
 - Tag - A name for the data
 - Label - A quoted string to be displayed with the data
 - Data type - One of a set of pre-defined labels for data types
 - Extra mainpulation and parsing options (must=, hidden=, enum=)

Offsets and sizes can be a numerical value, a tag (that references another
value), or a *simplified* mathematical expression. The mathematical expression
has an operator, along with left and a right operands, which can be numerical
values, tags, or other mathematical expressions. The basic syntax of a nested
mathematical expression using tags would look like this:

`( 2 + ( 5 * voffset ))`

The data type can be one of the following:
 - char - 8 bits, rendered as ASCII
 - uint8
 - int8
 - uint16
 - int16
 - uint32
 - int32
 - uint64
 - int64
 - ztstr - A zero-terminated string (still requires size)
 - flstr - A fixed length string

The extra options can be one of the following:
 - must= - The data in the field must equal a value (or processing will stop)
 - enum= - Specify an enum (string) to display, instead of the data
 - hidden= - Should the data be output or not (true/false)

### Enum definition

An enum definiton describes an enum for a parse point. They are specified on
a line of their own (not inline with the parse point), and have a specific syntax like this boolean enum:

`setenum mybenum = { default:On; 0:"Off" }`

Note in the above example that:
 - A default is always specified
 - Strings *can* be quoted, or not (if no spaces)
 - The enum definition is bounded by `{` and `}`
 - The `;` char divides items, but is optional on last item
 - The tag "mybenum" can be referenced in a parse point (using the enum= operator)
 - Numerical values can be in hex or dec

### Explicit tags

Parse points are implicit tags. A tag is created for each parse point that
can be used to reference the data elsewhere. Tags can be explicitly defined
using syntax similar to enums. It looks like this:

`settag mytag = 16`

Here the tag is assigned a value of 16. The assignment could be to any entity
using the format used for offsets and sizes in a parse point.

### Options

As the BPF file can be set executable and use file magic to find the bp
*interpreter*, options can be specified in the BPF file. Options are specified
with a setopt operator and then the options, much like they would be on the
command line, but without the dash options. If the option is a simple boolean
option, then it is followed by true or false. The following setopt option will
turn on verbose output (like -v would on the command line):

`setopt v true`

### Comments

Any text after a hash character (`#`) is a comment. Blank lines are ignored.

How it works - The developer view
---------------------------------

The BPF file is "compiled" in multiple passes/steps. They are:
 - Pass 0 - Read all setopt options from the BPF file
 - Pass 1 - Parse all enums, explicit tags, and parse points from the BPF file
 - Pass 2 - Resolve all tags
 - Pass 3 - Read data from binary file (in multiple "rakes"/passes of the
            parse points) to resolve the data

The code is laid out in "helper" and "stage related" source files. Comments can
be found in the source to find your way. 
