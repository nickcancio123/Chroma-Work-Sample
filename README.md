ABOUT THE PROJECT: 
This is a sample of some of the work I did on Chroma, my master's capstone project: a third-person rogue-lite shooter, developed in Unreal Engine 5. The project extensively uses Unreal's Gameplay Ability System (GAS), a flexible framework for implementing abilities, stats, and statuses. 

GAMEPLAY: 
Nadia, the main character, has a backpack with 6 batteries, which she can discharge one at a time with her primary weapon, a blaster. There are 7 types of batteries in the game, each with different abilities, that the player can swap in and out of her backpack. The player can collect upgrades for her batteries and movement abilities, allowing the player to customize their build.

BACKPACK AND BLASTER:
The general structure of this system goes as follows: the backpack has 6 batteries. Batteries have a simple API for altering their state: Recharge and Discharge, but many immutable accessors for the various fields associated with batteries. When the player presses the "shoot" input (i.e., left mouse button), it activates the "BlasterShoot" gameplay ability, which drives the functionality of shooting, changing the state of the backpack and the blaster through API calls. Some of the code for this system is written in Blueprint for quicker iteration and is denoted by a suffix of "_BP".

ABILITY RECHARGER:
This is just one example of how I extended the Gameplay Ability System for this project. This piece of code allows some abilities to have stacks (i.e., multiple charges), meaning that it can be activated multiple times in succession before having to go on a cooldown. Out of the box, GAS did not have this functionality, so, I had to make it in a modular way that works within the GAS framework. The idea is that an ability that stacks, say Nadia's dash, when activated, decrements the ability's charge count (a GAS attribute) and creates an instance of this AbilityRecharger, which asynchronously increments the charge count after a duration. 
