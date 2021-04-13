# DF_Patch_Stuff
Some patches for DeFRaG. Pretty much only contains custom `savepos` and `placeplayer` commands for now. They can save/restore:

- Timer
- Health
- Armor
- Items
- Weapons
- Frags
- You get the idea...

## Installation

Download [zzzzz-vm.pk3](./zzzzz-vm.pk3) and place it in your defrag folder.

## Usage

This was pretty much made for mapping/testing purposes. So:

- It can only be used in devmap (`sv_cheats 1`)
- To use it:
  1. `/set saveposname <whatever>`
  2. `/savepos`
  3. Tweak `<whatever>_*` cvars if you want (read the source code for format/order of each cvar)
  4. `/vstr <whatever>`

## Limitations

I would only use this for convenience (ie. when you don't have a bot/replayer or don't want to switch to it) and not for any ultra precise and accurate testing (just use the bot/replayer). Anyways, some of its limitations include:

- All the flaws of the original `savepos`/`placeplayer` (ie. ob) and possibly more...

- You can only use this on patched servers (you won't be able to `placeplayer` on unpatched servers)

- It doesn't store world state. For example:

  Suppose:

  - You `savepos` after firing a rocket/grenade/whatever
  - You restore using `/vstr <whatever>`

  Then:

  - You will restore with whatever ammo you had and everything
  - But the rocket/grenade/whatever you fired won't restore
  - It's possible to restore these, but at some point you are going to have to draw the line somewhere:
    - ie. Are you going to restore the map state too? The way the doors/triggers/etc. were?
    - Too much effort, too lazy, just believe in a replayer at that point

- It might stop you from using proxymods that use compile-time offsets or checksum the VM

  - The data offsets for the patched VMs should be the same as the original VMs

## Thanks

- Breadsticks
- Farter
- Horse
- Noobert
- pLuMbAgO
- PlUmBaGo

**Note:** There is supposed to be one more person here, but the proxy was so good, I have no idea who it is.