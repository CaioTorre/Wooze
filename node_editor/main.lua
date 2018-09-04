--local city_node = require('Node')
require('Node')
require('math')

function love.load()
	touch_state = false
	input_state = 0
	input_index = 0
	grab_index = 0
	drag_index = 0
	node_master = {}
	
	master_scale = 1
	
	xml_trig = true
	
	last_mouse_coords = { x = love.mouse.getX(), y = love.mouse.getY() }
	
	t = Node:new()
	table.insert(node_master, t)
	node_master[1]:setPos(50, 60)
end

function love.update()
	if xml_trig and love.keyboard.isDown("lctrl") and love.keyboard.isDown("lalt") and love.keyboard.isDown("return") then
		xml_trig = false
		print("Writing xml...")
		writeXML()
	end
	if not love.keyboard.isDown("lctrl") and not love.keyboard.isDown("lalt") and not love.keyboard.isDown("return") then
		xml_trig = true
	end
	if not touch_state then
		if love.keyboard.isDown("lshift") then
			if drag_index == 0 and love.mouse.isDown(1) then 
				touch_state = true
				mx, my = love.mouse.getPosition()
				for i, n in ipairs(node_master) do 
					if n:isTouched(mx, my, master_scale) then
						drag_index = i
					end
				end
			end
		else
			if love.mouse.isDown(1) then
				touch_state = true
				mx, my = love.mouse.getPosition()
				for i, n in ipairs(node_master) do 
					if n:touchUpperHalf(mx, my, master_scale) then
						input_state = 1
						input_index = i
						print("Left up touch at index "..i)
					elseif n:touchLowerHalf(mx, my, master_scale) then
						input_state = 2
						input_index = i
						print("Left down touch at index "..i)
					end
				end
			elseif grab_index == 0 and love.mouse.isDown(2) then
				touch_state = true
				mx, my = love.mouse.getPosition()
				for i, n in ipairs(node_master) do 
					if n:isTouched(mx, my, master_scale) then
						print("Right touch at index "..i)
						grab_index = i
						last_mouse_coords.x = mx
						last_mouse_coords.y = my
					end
				end
			end
		end
	end
	if not love.mouse.isDown(1) and drag_index > 0 then
		print("will try to add...")
		local done = false
		local mx, my = love.mouse.getPosition()
		for i, n in ipairs(node_master) do
			if not done and i ~= grab_index and n:isTouched(mx, my, master_scale) then
				print("found @ "..i)
				done = true
				node_master[drag_index]:addConnection(n.id, 50, "RUA")
			end
		end
	end
	if not love.mouse.isDown(1, 2, 3) then
		touch_state = false
		grab_index = 0 
		drag_index = 0
		--input_state = 0
	end
	if grab_index > 0 then
		local mx, my = love.mouse.getPosition()
		node_master[grab_index]:move(mx - last_mouse_coords.x, my - last_mouse_coords.y)
		last_mouse_coords.x = mx
		last_mouse_coords.y = my
	end
end

function love.draw()
	for i, n in ipairs(node_master) do
		n:draw(input_index == i, node_master, master_scale)
	end
	if drag_index > 0 then
		mx, my = love.mouse.getPosition()
		n = node_master[drag_index]
		love.graphics.setColor(1.0,0.5,0.5)
		love.graphics.line(n.x + n.wid/2, n.y + n.hei, mx, my)
	end
end

function love.textinput(key) 
	if key == "+" then
		table.insert(node_master, Node:new())
		node_master[#node_master]:setPos(300, 300)
		node_master[#node_master].id = #node_master - 1
		input_index = #node_master
	elseif key == "-" then
		table.remove(node_master, input_index)
	elseif input_state == 1 then
		node_master[input_index]:addToID(key)
	elseif input_state == 2 then
		node_master[input_index]:appendToName(key)
	end
end

function love.keypressed(key, scancode, isrepeat)
	if key == "return" then input_state = 0 end
	if key == "backspace" then
		if input_state == 1 then
			node_master[input_index]:delFromID()
		elseif input_state == 2 then
			node_master[input_index]:delFromName()
		end
	end
end

function love.wheelmoved(x, y)
	if y ~= 0 then
		local sz = 12
		local r = 0.1
		master_scale = math.max(master_scale + y * r, 0)
		love.graphics.setNewFont(12 * master_scale)
	end
end

function writeXML()
	file = love.filesystem.newFile("db.xml")
	file:open("w")
	file:write("<?xml version=\"1.0\" encoding=\"UTF8\">\n")
	for i, n in ipairs(node_master) do
		file:write(n:getXML())
	end
	file:close()
end
