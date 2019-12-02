#include "main.hpp"




Attack::Attack(const DamageType t_dmgType, const TCOD_dice_t t_dice, const char* t_verb) : m_dmgType{ t_dmgType }, m_dice{ t_dice }, m_verb{ t_verb } {}

const std::string& Attack::verb() { return m_verb; }

int Attack::rollDmg() { return Engine::s_rng->diceRoll(m_dice); }

DamageType Attack::dmgType()
{
	return m_dmgType;
}



ResistanceModel::ResistancePair::ResistancePair(float t_flat, float t_mult) :m_flat{ t_flat }, m_mult{ t_mult } {}

ResistanceModel::ResistanceModel(const DmgArr& t_flat, const DmgArr& t_mult) {
	for (int i{ 0 }; i < (int)DamageType::MAX_DAMAGE_TYPES; i++) {
		m_resistances.emplace((DamageType)i, ResistancePair{ t_flat[i],t_mult[i] });
	}

}

float ResistanceModel::flatRes(const DamageType t_dmgType)
{
	return m_resistances.at(t_dmgType).m_flat;
}

float ResistanceModel::dmgMult(const DamageType t_dmgType)
{
	return m_resistances.at(t_dmgType).m_mult;
}

float ResistanceModel::reduceDamge(const DamageType t_dmgType, const float t_dmgMagnitude)
{
	return (t_dmgMagnitude + m_resistances.at(t_dmgType).m_flat) * m_resistances.at(t_dmgType).m_mult;
}

void ResistanceModel::setFlatRes(const DmgArr& t_flat){
	for (int i{ 0 }; i < (int)DamageType::MAX_DAMAGE_TYPES; i++) {
		m_resistances.at((DamageType)i).m_flat = t_flat[i];
	}
}

void ResistanceModel::setFlatRes(const DamageType t_dmgType, const float t_value){
	m_resistances.at(t_dmgType).m_flat = t_value;
}

void ResistanceModel::setMultRes(const DmgArr& t_mult){
	for (int i{ 0 }; i < (int)DamageType::MAX_DAMAGE_TYPES; i++) {
		m_resistances.at((DamageType)i).m_mult = t_mult[i];
	}
}

void ResistanceModel::setMultRes(const DamageType t_dmgType, const float t_value){
	m_resistances.at(t_dmgType).m_mult = t_value;
}