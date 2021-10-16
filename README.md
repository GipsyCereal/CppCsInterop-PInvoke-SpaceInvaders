# CppCsInterop-PInvoke-SpaceInvaders

## What is interoperability?
the capability of two different programming languages to natively interact as part of the same system and operate on the same kind of data structures. In this case it's unmanaged C++ data being marshalled into managed C# data using PInvoke.

### PInvoke?
Explicit Platform Invoke (or PInvoke) is, in broad strokes, a feature that allows managed code to call unmanaged "native" code. explicit PInvoke uses the DllImport attribute.
Pros for using explicit PInvoke:
 -Not platform specific.
 -No real need for an intermediate library (you can directly call the managed 
functions)
Cons for using explicit PInvoke:
 -Has a more marshalling overhead causing lesser performance than implicit invoking, this does not matter in many cases.
 
## Why?
Sometimes, we want code that has been written and compiled in different programming languages to communicate with each other. The Unity engine, for example, is written in C++, but the editor is mainly written in C#.
Unity exposes a .NET API so that not every programmer is required to write their game in C++, but in C# instead.
The space invaders logic is simple, yet it taught me much about PInvoke interoperability.

## The Project!
