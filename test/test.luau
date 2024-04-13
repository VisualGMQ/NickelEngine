function on_init(entity:number): nil
    t = nickel.ecs.GetTransform(0)
    s = nickel.ecs.GetSprite(0)

    t.scale.x = 5
    t.scale.y = 5
end

function on_update(entity:number): nil
    if nickel.ecs.res.Keyboard:Key(nickel.Key.D):IsPressing() then
        t.translation.x = t.translation.x + 1;
        s.flip = nickel.Flip.None
    elseif nickel.ecs.res.Keyboard:Key(nickel.Key.A):IsPressing() then
        t.translation.x = t.translation.x - 1;
        s.flip = nickel.Flip.Horizontal
    end
end