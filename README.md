# RelyConn chat program

This project is for learning purposes: it's aim is to apply, from theory, multithread socket communication with encryption. The program
works on connecting one or more users to a central server, where they can communicate with each other. To communicate the system uses a login procedure: it
stores the username on the server, which serves as an id, that can be used to redirect the message to the user with that name.

It uses C as a programming language with the built in pthread and mutex for multithreading handling; 

**Screenshot**: If the software has visual components, place a screenshot after the description; e.g.,

![](https://raw.githubusercontent.com/cfpb/open-source-project-template/main/screenshot.png)

##Platforms

The program is only tested to work on Ubuntu linux. Other systems need a configuration which has not been implemented yet
## Dependencies

these dependencies are necessary to be installed in to the machine to make the program work. These are:
[GNU-MP]()
if that doesn't work you can try to install it with this in CLI: sudo apt-get install libgmp3-dev libgmp10
[tiny-AES-c]

## Installation

Detailed instructions on how to install, configure, and get the project running.
This should be frequently tested to ensure reliability. Alternatively, link to
a separate [INSTALL](INSTALL.md) document.

## Configuration

If the software is configurable, describe it in detail, either here or in other documentation to which you link.

## Usage

Show users how to use the software.
Be specific.
Use appropriate formatting when showing code snippets.

## How to test the software

If the software includes automated tests, detail how to run those tests.

## Known issues

Document any known significant shortcomings with the software.

## Getting help

Instruct users how to get help with this software; this might include links to an issue tracker, wiki, mailing list, etc.

**Example**

If you have questions, concerns, bug reports, etc, please file an issue in this repository's Issue Tracker.

## Getting involved

This section should detail why people should get involved and describe key areas you are
currently focusing on; e.g., trying to get feedback on features, fixing certain bugs, building
important pieces, etc.

General instructions on _how_ to contribute should be stated with a link to [CONTRIBUTING](CONTRIBUTING.md).


----

## Open source licensing info
1. [TERMS](TERMS.md)
2. [LICENSE](LICENSE)
3. [CFPB Source Code Policy](https://github.com/cfpb/source-code-policy/)


----

## Credits and references

1. Projects that inspired you
2. Related projects
3. Books, papers, talks, or other sources that have meaningful impact or influence on this project
