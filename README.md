# DF_Patch_Stuff
Some patches for DeFRaG. Pretty much only contains custom `savestate` and `restorestate` commands for now. They can save/restore:

- Timer
- Health
- Armor
- Items
- Weapons
- Frags
- You get the idea...

## Installation

// TODO (#12): Fill this in

## Usage

This was pretty much made for mapping/testing purposes. So:

- It can only be used in devmap (`sv_cheats 1`)
- To use it:
  1. `/set savestatename <whatever>`
  2. `/savestate`
  3. Tweak `<whatever>_*` cvars if you want (read the source code for format/order of each cvar)
  4. `/vstr <whatever>`

## Limitations

I would only use this for convenience (ie. when you don't have a bot/replayer or don't want to switch to it) and not for any ultra precise and accurate testing (just use the bot/replayer). Anyways, some of its limitations include:

- All the flaws of the original `savestate`/`restorestate` (ie. ob) and possibly more...

- You can only use this on patched servers (you won't be able to `restorestate` on unpatched servers)

- It doesn't store world state. For example:

  Suppose:

  - You `savestate` after firing a rocket/grenade/whatever
  - You restore using `/vstr <whatever>`

  Then:

  - You will restore with whatever ammo you had and everything
  - But the rocket/grenade/whatever you fired won't restore
  - It's possible to restore these, but at some point you are going to have to draw the line somewhere:
    - Are you going to restore the way the doors/triggers/etc were?
    - Too much effort, too lazy, just believe in a replayer at that point

- Restoring in fastcaps is a bit broken depending on how you view it:

  Suppose:

  - You are on the red team and have the blue flag
  - You `savestate`, switch to blue team, and `vstr`

  Then:

  - You will restore as a blue team member holding the blue flag :D
  - I'm not sure what the expected behavior would be for such a situation
  - I could always restore you with the opposing team flag instead of whatever was saved at the time of `savestate`
  - Too lazy though, you can manually toggle two bits in `<whatever>_items` for now

- It might stop you from using proxymods/cheat engines that checksum the VM or have hardcoded pre 1.91.27 offsets

  - The data offsets for the patched VMs should be the same as the original VMs

## Thanks

- Breadsticks
- Farter
- Horse
- Noobert
- pLuMbAgO
- PlUmBaGo

**Note:** There is supposed to be one more person here, but the proxy was so good, I have no idea who it is.