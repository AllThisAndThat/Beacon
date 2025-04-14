cd C:\Users\lucas\OneDrive\Documents\GitHub\Beep-Boop\Beacon_Multiboard

kikit separate --source "annotation; ref: Main" 		Beacon_Multiboard.kicad_pcb 				Boards\Main\Main_Board.kicad_pcb
::kikit fab jlcpcb --no-drc --no-assembly 				Boards\Main\Main_Board.kicad_pcb			Boards\Main

kikit separate --source "annotation; ref: Tactile" 		Beacon_Multiboard.kicad_pcb 				Boards\Tactile\Tactile_Board.kicad_pcb
::kikit fab jlcpcb --no-drc --no-assembly 				Boards\Tactile\Tactile_Board.kicad_pcb 		Boards\Tactile

kikit separate --source "annotation; ref: Touch" 		Beacon_Multiboard.kicad_pcb 				Boards\Touch\Touch_Board.kicad_pcb
::kikit fab jlcpcb --no-drc --no-assembly 				Boards\Touch\Touch_Board.kicad_pcb 			Boards\Touch

kikit separate --source "annotation; ref: FLED" 		Beacon_Multiboard.kicad_pcb 				Boards\FLED\FLED_Board.kicad_pcb
::kikit fab jlcpcb --no-drc --no-assembly 				Boards\FLED\FLED_Board.kicad_pcb			Boards\FLED

kikit separate --source "annotation; ref: Audio" 		Beacon_Multiboard.kicad_pcb 				Boards\Audio\Audio_Board.kicad_pcb
::kikit fab jlcpcb --no-drc --no-assembly 				Boards\Audio\Audio_Board.kicad_pcb 			Boards\Audio