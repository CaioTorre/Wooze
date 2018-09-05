require('math')

Node = {
	xpos = 0, ypos = 0, id = 0, nome = "NULL", conns = {}
}

function Node:new(n)
	n = n or {}
	
	o = {
		x = n.x or 0,
		y = n.y or 0,
		wid = n.wid or 300,
		hei = n.hei or 50,
		id = n.id or 0,
		nome = n.nome or "NULL",
		conns = n.conns or {}
	}
	
	self.__index = self
	return setmetatable(o, self)
end

function Node:draw(isSelected, n_master, scale)
	if isSelected then
		love.graphics.setColor(0.7, 0.7, 0.4)
	else
		love.graphics.setColor(0.5, 1.0, 1.0)
	end
	love.graphics.rectangle("fill", self.x, self.y, self.wid * scale, self.hei * scale)
	love.graphics.setColor(0.1, 0.1, 0.1)
	love.graphics.printf("Id: "..self.id, self.x + 10 * scale, self.y + 10 * scale, self.wid * scale)
	love.graphics.printf("Nome: "..self.nome, self.x + 10 * scale, self.y + 30 * scale, self.wid * scale)
	
	love.graphics.setColor(1.0,0.5,0.5)
	for i, c in ipairs(self.conns) do
		local tx, ty
		for k, n in ipairs(n_master) do
			if c.toID == n.id then
				tx = n.x + n.wid/2
				ty = n.y
			end
		end
		love.graphics.line(tx, ty, self.x+self.wid * scale/2, self.y+self.hei * scale)
	end
end

function Node:isTouched(tx, ty, scale)
	local w = self:scaleWidth(scale)
	local h = self:scaleHeight(scale)
	return (tx > self.x and tx < self.x + w and ty > self.y and ty < self.y + h)
end

function Node:touchUpperHalf(tx, ty, scale)
	local w = self:scaleWidth(scale)
	local h = self:scaleHeight(scale)
	return (tx > self.x and tx < self.x + w and ty > self.y and ty < self.y + h/2)
end

function Node:touchLowerHalf(tx, ty, scale)
	local w = self:scaleWidth(scale)
	local h = self:scaleHeight(scale)
	return (tx > self.x and tx < self.x + w and ty > self.y + h/2 and ty < self.y + h)
end

function Node:scaleWidth(scale)
	return self.wid * scale
end

function Node:scaleHeight(scale)
	return self.hei * scale
end

function Node:move(x, y)
	self.x = self.x + x
	self.y = self.y + y
end

function Node:setPos(x, y)
	self.x = x
	self.y = y
end

function Node:appendToName(c)
	self.nome = self.nome..c
end

function Node:delFromName()
	self.nome = self.nome:sub(1, -2)
end

function Node:addToID(n)
	if n == "0" then
		v = 0
	elseif n == "1" then
		v = 1
	elseif n == "2" then
		v = 2
	elseif n == "3" then
		v = 3
	elseif n == "4" then
		v = 4
	elseif n == "5" then
		v = 5
	elseif n == "6" then
		v = 6
	elseif n == "7" then
		v = 7
	elseif n == "8" then
		v = 8
	elseif n == "9" then
		v = 9
	else return end
	self.id = self.id * 10 + v
end

function Node:delFromID()
	self.id = math.floor(self.id / 10)
end

function Node:addConnection(toID, weight, name)
	table.insert(self.conns, {
		toID = toID or 0, 
		weight = weight or 50, 
		name = name or "RUA"
	})
end

function Node:getXML()
	local t = "\t<node id=\""..self.id.."\" nome=\""..self.nome.."\">\n"
	for i, c in ipairs(self.conns) do
		t = t.."\t\t<rua peso=\""..c.weight.."\" id=\""..c.toID.."\" nome=\""..c.name.."\"></rua>\n"
	end
	return t.."\t</node>\n"
end
