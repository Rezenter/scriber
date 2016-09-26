# scriber
Qt-based scriber software and arduino firmware

Scriber help:

	used files:
	
		upper programm uses "default.save" file, file can be edited as .txt
			! without correct data in this file programm will be terminated !
			here is exampe of the file:
				"
					# positionY = 15
					# shape = rectangle
					# lastUsedPort = COM41
					# windowSizeY = 536
					# rectangleX = 60.00
					# positionX = 20
					# windowSizeX = 570
					# step = 10.00
					# realLengthX = 65.3
					# windowTitle = scriber
					# rectangleY = 48.00
					# radius = 10.00
					# cutter = R
					# realLengthY = 72.2
				"
			strings order can be messed up
			file will be overwriten at the end of the session
		
		upper programm creates "log.txt" file with current session log,
			file will be overwriten at the end of the session
			
		in the folder there is this "help.txt" file
		
		also the folder contains raw qt-code files and arduino .ino firmware
		
	hardware:
	
		currently scriber is controlled by arduino nano rev3
		
		! after any intervention in opt-sensors system, you MUST manually measure real length of x and y movement
		and edit corresponding values in "default.save" file, while the upper programm is TERMINATED, otherwise it will rewrite your changes
		!
		
	interface:
	
		"settings" page:
			
			"scan" button:
				
				refreshes COM-ports list, 
				automatically called at programm start.
				
				always enabled
				
			"COM*" comboBox:
				
				contains availiable COM-ports found when "scan" performed
				saves last used port and selects it, if it`s availiable at startup.
				
				always enabled
			
			"connect" button:
				
				closes previously used port,
				opens selected in comboBox port,
				sends "ping(0)" to arduino,
				listens for correct response.
				
				! may cause arduino reboot !
				
				makes "next" button unenabled, until correct response received.
				
				always enabled
				
			"calibrate" button:
				
				sends "calibrate(0)" signal to arduino,
				arduino moves both steppers in order to find all opt-sensors,
				sets steppers zero and limits.
				
				after correct response makes "next" button enabled.
				
				enabled after "connect" button pressed and ping received
				
			"substrate shape" group:
				
				last checked radio button saved
				
				"round" radio buton:
					
					used with round-shaped substrate,
					if checked, calls round-cutting logic
					
					enables "auto cut" button
					
					always enabled
					
					exclusive with "rectangle" and "else"
					
				"rectangle" radio button:
					
					used with rectangle-shaped substrate,
					if checked, calls rectangle-cutting logic
					
					enables "auto cut" button
					
					always enabled
					
					exclusive with "round" and "else"
					
				"else" radio button:
				
					can be used with any substrate shape,
					
					! disables "auto cut" button
					
					exclusive with "round" and "rectangle"
					
				radius input line:
					
					holds round-shaped substrate radius [mm],
					editable, can hold up to 5 symbols
					
					! only digits and dot as a separator can be written
					
					saves last used value
					
					always editable
					
				x input line:
					
					holds rectangle-shaped substrate x-length [mm],
					editable, can hold up to 5 symbols
					
					! only digits and dot as a separator can be written
					
					saves last used value
					
					always editable
					
				y input line:
					
					holds rectangle-shaped substrate y-length [mm],
					editable, can hold up to 5 symbols
					
					! only digits and dot as a separator can be written
					
					saves last used value
					
					always editable
					
			"use left cutter" radio buton:
				
				if checked, left cutter will be used
				
				exclusive with "use right cutter"
				
				always editabled
				
			"use right cutter" radio buton:
				
				if checked, right cutter will be used
				
				exclusive with "use left cutter"
				
				always editabled
				
			"next" button:
				
				shows "positioning & cutting" page
				
				enabld only after calibration
				
		"positioning & cutting" page:
		
			"moving controls" group:
			
				"absolute" group:
				
					"step" group:
						
						"move X" button:
						
							sets nearby line value as x-steppers destination
							
							unenabled while cutting
						
						x line:
							
							holds absolute x-steppeer destination coordinate in steps
							
							always editable, can hold up to 5 digits with sign
							
						"move Y" button:
						
							sets nearby line value as y-steppers destination
							
							unenabled while cutting
						
						y line:
							
							holds absolute y-steppeer destination coordinate in steps
							
							always editable, can hold up to 4 digits
					
					"mm" group:
						
						"move X" button:
						
							sets nearby line value as x-steppers destination
							
							unenabled while cutting
						
						x line:
							
							holds absolute x-steppeer destination coordinate in mm
							
							always editable, can hold up to 6 digits with sign and dot separator
							
						"move Y" button:
						
							sets nearby line value as y-steppers destination
							
							unenabled while cutting
						
						y line:
							
							holds absolute y-steppeer destination coordinate in mm
							
							always editable, can hold up to 5 digits with dot separator
				
				"relative" group:
				
					"step" group:
						
						"move X" button:
						
							sets nearby line value as x-steppers destination
							
							unenabled while cutting
						
						x line:
							
							holds relative x-steppeer destination coordinate in steps
							
							always editable, can hold up to 6 digits with sign
							
						"move Y" button:
						
							sets nearby line value as y-steppers destination
							
							unenabled while cutting
						
						y line:
							
							holds relative y-steppeer destination coordinate in steps
							
							always editable, can hold up to 5 digits with sign
					
					"mm" group:
						
						"move X" button:
						
							sets nearby line value as x-steppers destination
							
							unenabled while cutting
						
						x line:
							
							holds relative x-steppeer destination coordinate in mm
							
							always editable, can hold up to 6 digits with sign and dot separator
							
						"move Y" button:
						
							sets nearby line value as y-steppers destination
							
							unenabled while cutting
						
						y line:
							
							holds relative y-steppeer destination coordinate in mm
							
							always editable, can hold up to 6 digits with sign and dot separator
						
				"stop X" button:
					
					stops the x-steppeer as fast, as possible using maxAccelerationX value in arduino
					
				"stop Y" button:
					
					stops the y-steppeer as fast, as possible using maxAccelerationY value in arduino
				
				"park" button:
					
					sets x-steppeer destination to zero in absolute steps,
					sets y-steppeer destination as far from zero, as possible.
					
			"cutting controls" group:
			
				"single cut" button:
					
					lands selected at the "setting" page cutter at current point,
					moves y-steppeer to the zero or till the cutter drop, both cause cutter rising,
					then moves cutter to the cutting start point and lands cutter again,
					then moves y-steppeer to the maxYCoordinate or till the drop, both cause cutter rising,
					finally sets the cutting start point as y-steppers destination and 
					move x-steppeer right to "step" line
					
					makes "moving controls" and "back" unenabled till the end of the cutting
					
					unenabled while moving
					
				step line:
					
					holds length between cuts
					can hold up to 5  digits with dot separator
					always editable
					saves last used value
					
				"auto cut" button:
				
					! not displayed, if "else" radio button is checked !
					
					if "round" is checked, performs (radius/step) cuts
					if "rectangle" is checked, performs (x/step) cuts,
					and shows "swap x/y" dialog, wich will swap x & y values on "settings" page
					
					unenabled while moving
					
			"y limits":
			
				shiows maximum(top) and minimum (bottom) absolute Y coordinates in steps and mm
					
			"x limits":
			
				shiows maximum(right) and minimum (left) absolute X coordinates in steps and mm
			
			"abort" button:
				
				calls stopX(), stopY, and stops cutting
				
			"back" button:
			
				return to the "setings page"
				
				unenabled while cutting
				
		log text field:
			
			contains copy of logFile, scrollable
			
		command line:
			
			! use only if you`re 100% sure in your actions !
			
			holds text commands
			
		"send" button:
		
			! use only if you`re 100% sure in your actions !
			
			send command to arduino, bypassing upper programms protection
			
			list of valid commands:
				
				move(2, char z, int coord)
					
					move x or y stepper to the coord
					! stepper coords differ from upper programs ones !
					incorrect coord will be responsed by arduino with "incorrectCoordZ(int xLength, int given coord)", 
					where z = 'x' or 'y'
					
				stop(1, char z)
					
					where z = 'x' or 'y'
					stops selected stepper as fast, as possible using preprogrammed acceleration
				
				land(1, char z)
				
					where z = 'l' or 'r'
					set selected relay to HIGH state
					
				rise(1, char z)
					where z = 'l' or 'r'
					set selected relay to LOW state
				
				calibrate(0)
					calls "calibration()" void preprogrammed in arduino
					
				ping(0)
					refreshes state of arduino, sets both relays to LOW state, stops both steppers, sends "connected()" back to PC
				
				speed(2, char z, int sp)
					where z = 'x' or 'y'
					sets selected steppers maxSpeed to sp
					
				every else commands will be responsed by arduino with "incorrectCommand(String received)"
				misspelled command will be responsed by arduino wuth "incorrectCommand(\" + String received + \")"

	

	if there`re any questions regarding scriber and/or its software you can contact with me by:
		e-mail(good) = nisovru@gmail.com
		phone(bad) = +7 911 23 21 572
		vk (the worst) = http://vk.com/rezenter
