# Code Entropy Meter
Copyright (c) 2014-2018 by Andrej Nichelmann and Klaus Michael Indlekofer. All rights reserved.
Note: Special restrictions apply. See disclaimers below and within the distribution.

## Summary
Code Entropy Meter is a real-time audio analysis application which offers the following features:
* Bit-accurate visualization so that you can find out whether your audio hardware is using the full bit depth range.
* Entropy calculation with variable block size. 
* Peak, RMS and Crest factor measurement.
* Support for all common host access audio API thanks to portaudio. On Windows that would be: MME, DirectSound, WASAPI, WDM-KS and Steinberg ASIO. Any hardware's supported bit depth, sample rate and channel can be chosen.
IMPORTANT: Currently tested only on Windows.

This project is currently hosted on:
* GitHub: https://github.com/andnich05/codeentropymeter

This software is OPEN SOURCE and is published under the GNU GENERAL PUBLIC LICENSE (see COPYING for details)

This project uses:
* Qt - https://www.qt.io
* Portaudio - http://www.portaudio.com/
* Steinberg ASIO - https://www.steinberg.net/de/company/developer.html

## Compiling
To build the application by yourself on Windows you will need the following:

* Microsoft Visual Studio 2017 or newer (Community version is fine)
* Qt 5 (tested with Qt 5.10)
* Portaudio (tested with pa_stable_v190600_20161030)
* Steinberg ASIO SDK (optional when building portaudio)

You will need a dynamic portaudio library (.dll), either build it yourself (http://portaudio.com/docs/v19-doxydocs/compile_windows.html) or get a pre-built version. 
Put the portaudio_x64.dll/portaudio_x86.dll and portaudio_x64.lib/portaudio_x86.lib library files into lib/portaudio and the portaudio.h into include/portaudio. Open the QtCreator project, configure it and hit Build.

## Installation

You may need the appropriate Visual C++ Redistributable Package 2017 (x86 or x64):
https://www.visualstudio.com/downloads/

Just start code-entropy-meter.exe in the main directory.

## Contact

Andrej Nichelmann (andnich05) - andnich05dev@gmail.com
Klaus Michael Indlekofer (kmi9000) - m.indlekofer@gmx.de

---
The following holds for all files in this distribution (unless stated otherwise on an
individual basis for each file and statement):

These files are distributed in the
hope that they will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. In no event shall the authors be
liable for any direct, indirect, incidental, special, exemplary, or consequential damages
(including, but not limited to, procurement of substitue goods or services; loss of use,
data, or profits; or business interruption) however caused and on any theory of liability,
whether in contract, strict liability, or tort (including negligence or otherwise) arising
in any way out of the use of data/information/software from this distribution, even if
advised of the possibility of such damage. 

The contents of this distribution are intended for educational, non-commercial purposes
only. Materials contained herein are property of their respective owners. If any
copyrighted works/trademarks have been used, please contact the authors A. Nichelmann and K. M. Indlekofer
and the item will be either removed or properly credited (at the copyright/trademark
owner's discretion). We have no intention of violating any copyrights or trademarks.
This distribution might use inlining and deep-linking, i.e. links in this distribution
might lead directly to materials on other web sites/distributions (in which case the
target page normally should be listed/credited in a "links" section). The authors do
not take responsibility for the contents of any links referred to. We do not necessarily 
endorse, sanction, support, encourage, verify or agree with the contents, opinions or
statements of/on any of the linked pages. These statements hold for all links/references
in all files in this distribution. We are in no way affiliated with any
companies/institutions/individuals which might be mentioned in any manner in this
distribution.

The authors do not take responsibility for incorrect, incomplete or misleading information.
Statements are to be considered as the authors' free personal opinion. The authors do not
necessarly possess any of the items mentioned in files in this distribution.

Files (and the information therein) created by the authors A.Nichelmann and K. M. Indlekofer are copyright
(c) by A. Nichelmann and K. M. Indlekofer. Unless protected/restricted otherwise, the authors permit
reproduction/redistribution of material contained in this distribution under the condition
that the item is properly credited. Links to items/materials in this distribution are welcome.
Projects/publications/papers that make use of materials, programs, or generated output
of this distribution must properly credit the authors and mention the usage of this distribution.
Please contact the authors A. Nichelmann and K. M. Indlekofer for comments or further questions
and permission to use materials/information from this distribution.

Some programs/data files are released under the terms of the GNU General Public License
as published by the Free Software Foundation. (Address: Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. Internet: GNU General Public
License (GPL) from The GNU Project) 
---
