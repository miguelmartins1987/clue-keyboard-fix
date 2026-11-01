# What is this?

A fix for Clue: Murder at Boddy Mansion running on Windows 11. The problem is that, in some setups (mine included), one cannot enter their name at the character selection screen. This fixes that by detouring `ToAsciiEx` using Microsoft Detours.

# Where can I grab it?

Get whichever version is the latest at: https://github.com/miguelmartins1987/clue-keyboard-fix/releases

# Any other fixes you recommend for running the game?

As a matter of fact, yes. After installing DirectPlay (which is a must for the game to even boot), I find myself with some sort of "Fatal Access" error. To correct this, I grab a copy of FunkyFr3sh's cnc-ddraw at https://github.com/FunkyFr3sh/cnc-ddraw/releases and extract its files to the game's folder (usually: C:\Program Files (x86)\Hasbro Interactive\Clue). You may then configure cnc-ddraw using the provided **cnc-ddraw config.exe**. Here's what I typically use for configuration:

<img width="1118" height="768" alt="image" src="https://github.com/user-attachments/assets/7e56e307-b943-405c-88e7-fae852184510" />


# Are there alternative fixes for this problem?

Certainly! If you don't like using Detours for whatever reason, check out: https://classicgamefixes.wixsite.com/home/clue-murder-at-boddy-mansion

That is essentially a patch created via the Microsoft Application Compatibility Toolkit (ACT), and it should work as well.
