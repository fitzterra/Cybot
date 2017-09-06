Tests
=====

This directory contains sub-directories that are used to test individual
features and functions as they are developed in an isolated environment.

Because the software is very modular, it is easy to test, and develop,
individual functionality outside of the main code body.

This is done by creating a sub-directory here with a proper name to define the
feature/function being tested or developed in isolation, then creating relative
symlinks (`../../file.name`) to only the files needed.

Symlinking the `Makefile` is also a good idea, but in certain circumstance it
may be required to have a specific Makefile for the test/feature. In these cases
it may also be an option to have a local Makefile that includes the main one,
and just overrides what is needed.

The benefit of using symlinks is that it allows changing and updating the files
while developing, which automatically does the changes in the main code body. No
worries of having to copy files and possibly get them out of sync.


