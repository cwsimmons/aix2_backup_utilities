# IBM AIX 2 Backup File Extraction Utility

## Overview

This program can extract the contents of dump files created by the `backup` command.

## Usage Notes

* The backup may be stored across multiple volumes. Each argument passed to this
program should be the file name of a volume. They will be processed in the order given.

## Limitations

* All files are created in the current working directory. If a file name is an absolute path the program will attempt to use it as-is, which is probably not desirable behavior.
* File time stamps and permission flags are not set to their values from the backup
* Only "by name" backups are supported.

## License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
