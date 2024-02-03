function on_init(): nil
end

function on_update(): nil
    local v = nickel.cgmath.Vec2(1, 2)
    print(v)

    local s = nickel.Sprite()
    print(s.texture)
    s.customSize = nickel.cgmath.Vec2(3, 3)
    print(s.customSize)
    s.flip = nickel.Flip.Vertical
    print(s.flip)
end