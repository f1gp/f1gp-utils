/*
** Version (change MAGIC2 if saved games are incompatible).
**
** V0.1 06/06/94    First bash.
** V0.2 12/06/94    Save all registers for hook functions.
** V1.0 31/06/94    Added tracking of last pit.
** V2.0 30/08/94    Fixed "pit request" not being cleared bug.
** V3.0 29/10/94    Added tyre handling and multi-player support.
** V4.0 16/01/20    Added support for replays. Not compatible with old save games.
**                  Added support for specific tyre compound per pit group and pit stop.
**                  Increased maximum number of pit groups and pit stops.
**                  Improved handling of unscheduled pit stops.
**                  Prints actual pit group configuration.
**                  Added support for configuration files.
**                  Preselect tyre for player in control during multiplayer.
**                  Set minimum race distance to 5% instead of 75%.
**                  Added option -#N to assign a specific car number to a pit group.
**                  Changed option -lN to -%N.
**                  Allow to skip dashes for group options.
*/

#define VERSION     "V4.0.6 16/Jan/2020"

