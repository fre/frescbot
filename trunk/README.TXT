-*- outline -*-

  FreScBot v1.0
  Copyright Florent D'Halluin, Valentin Leon-Bonnet 2010

  This is a simple micro bot for the Starcraft AI Competition (AIIDE2010).
  See http://eis.ucsc.edu/StarCraftAICompetition.
  
  This AI is organized around a multi-agent structure:
  Each individual unit is a state-machine agent that takes decisions (i.e.
  decides where to go and what to shoot at) based on its environment.
  Some higher-level structure (UnitSquad, TacticalManager) provide more
  strategic guidance.
  
  This AI was meant to participate in all tournaments, but only the
  micromanagement aspect was implemented and functional in time.  For this
  reason there are a lot on unused/barely used classes in the source.
  
  Most of the interesting stuff happens in SmartAgent.cc/hh.

  COPYRIGHT NOTICE
------------------------------------------------------------------------
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------------------------------


** The rest of this file is outdated **


Starcraft AI Bot FreAIModule
Version 0.1

Simple combat microgestion bot.

** Basic organization **

 A. Time Manager.
 B. Information Manager.
 C. Unit Agents.


A. Time Manager:

  Timing and performance measurement.  Limit the AI resource usage in the main
  thread.
  
  Notes:
   - Currently no action is taken if the AI uses too much resource.

  
B. Information Manager:

  Maintain the state of all the units on the map.  Provide a list of targets
  and threats.

  Notes:
   - Eventually, infer opponent's strategy.
  

C. Unit Agents:

  Each unit is an independent agent and issues its own order.
  
  Available agents:
   - UnitAgent:
     Base Agent, does nothing.
   - RangedUnitAgent:
     Basic combat behavior, prioritizes weak targets in range.

  Notes:
   - Eventually, many types of agents.
   - Eventually, agents can adapt their behavior.


** Implementation details **

Dependencies:
 - BWAPI 2.6.1

Development notes:
 - All manager methods and variables are static.
 - Windows naming conventions: MyType myVar.
