#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "Game.h"
#include <vector>
#include "facilities/Facility.h"
#include "Monster.h"
#include "Food.h"

class Player
{
public:
	static Player* getPlayer(){
		static Player p;
		return &p;
	};
	void load();
	void update();
	void write();
	/*
	* @brief responsible of the Game::state change
	*/
	void setrequest(Game::STATE s){ req=s;}
	Game::STATE getRequest(){return req;}
	void setAcessID(int id){ accessID=id;}
	void setAcessFacIdx(int id){ fac_idx=id;}
	int getAcessID(){ return accessID;}
	int getAcessFacIdx(){ return fac_idx;}
	std::vector<Facility>& getFacilities(){ return land_settings;}
	std::vector<Monster>& getMonsters(){ return monster_owned;}
	std::vector<Monster>& getAllMonsters(){ return all_monsters;}
	std::unordered_map<Food::TYPE_F, Food>& getAllFoods(){ return all_food;}
	bool loadFacilties();
	bool saveFacilities();
	bool loadUserMonsters();
	bool saveUserMonsters();
	bool loadAllMonsters();
	bool loadAllFoods();
	

	int& getCoin(){ return coin;}
	int& getBer(){ return berries;}
	int& getExp(){ return exp;}
	int& getLevel(){ return level;}

private:
	Game::STATE req;
	int berries;
	int coin;
	int exp;
	int level;
	int fac_idx;
	std::vector<Facility> land_settings;
	std::vector<Monster> monster_owned;
	std::vector<Monster> all_monsters;
	std::unordered_map<Food::TYPE_F, Food> all_food;
	const int MAX_TYPE_OF_MONSTERS = 2;
	const int MAX_LAND = 8;
	int accessID;
	
};

#endif
