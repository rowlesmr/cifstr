#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <unordered_map>
#include <format>
#include <numbers>
#include <algorithm>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <cmath>


inline static constexpr double deg{ 180.0 / std::numbers::pi };

inline static constexpr double as_B{ 8 * std::numbers::pi * std::numbers::pi };

inline static constexpr std::array<std::string_view, 2> NA_values{ ".", "?" };

inline static constexpr const std::array<std::string_view, 100> elements
{ "Ac", "Ag", "Al", "Am", "Ar", "As", "At", "Au", "Ba", "B",
  "Be", "Bi", "Br", "Ca", "Cd", "C",  "Ce", "Cl", "Co", "Cr",
  "Cs", "Cu", "Dy", "Er", "Eu", "F",  "Fe", "Fr", "Ga", "Gd",
  "Ge", "H",  "He", "Hg", "Hf", "Ho", "I",  "In", "Ir", "K",
  "Kr", "La", "Li", "Lu", "Mg", "Mn", "Mo", "N",  "Na", "Nb",
  "Nd", "Ne", "Ni", "O",  "Os", "Pb", "P",  "Pa", "Pd", "Po",
  "Pr", "Pm", "Pt", "Ra", "Rb", "Re", "Rh", "Rn", "Ru", "S",
  "Sb", "Sc", "Sm", "Se", "Si", "Sn", "Sr", "Ta", "Tb", "Tc",
  "Te", "Th", "Tl", "Ti", "Tm", "W",  "U",  "V",  "Xe", "Y",
  "Yb", "Zn", "Zr", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "D" };

inline static constexpr const std::array<std::string_view, 3> water{ "Wat", "WAT", "wat" };

inline static constexpr const std::array<std::string_view, 212> allowed_atoms
{ "D",    "H",    "H-1",  "D-1",  "He",   "Li",   "Li+1", "Be",   "Be+2", "B",
  "C",    "N",    "O",    "O-1",  "O-2",  "F",    "F-1",  "Ne",   "Na",   "Na+1",
  "Mg",   "Mg+2", "Al",   "Al+3", "Si",   "Si+4", "P",    "S",    "Cl",   "Cl-1",
  "Ar",   "K",    "K+1",  "Ca",   "Ca+2", "Sc",   "Sc+3", "Ti",   "Ti+2", "Ti+3",
  "Ti+4", "V",    "V+2",  "V+3",  "V+5",  "Cr",   "Cr+2", "Cr+3", "Mn",   "Mn+2",
  "Mn+3", "Mn+4", "Fe",   "Fe+2", "Fe+3", "Co",   "Co+2", "Co+3", "Ni",   "Ni+2",
  "Ni+3", "Cu",   "Cu+1", "Cu+2", "Zn",   "Zn+2", "Ga",   "Ga+3", "Ge",   "Ge+4",
  "As",   "Se",   "Br",   "Br-1", "Kr",   "Rb",   "Rb+1", "Sr",   "Sr+2", "Y",
  "Y+3",  "Zr",   "Zr+4", "Nb",   "Nb+3", "Nb+5", "Mo",   "Mo+3", "Mo+5", "Mo+6",
  "Tc",   "Ru",   "Ru+3", "Ru+4", "Rh",   "Rh+3", "Rh+4", "Pd",   "Pd+2", "Pd+4",
  "Ag",   "Ag+1", "Ag+2", "Cd",   "Cd+2", "In",   "In+3", "Sn",   "Sn+2", "Sn+4",
  "Sb",   "Sb+3", "Sb+5", "Te",   "I",    "I-1",  "Xe",   "Cs",   "Cs+1", "Ba",
  "Ba+2", "La",   "La+3", "Ce",   "Ce+3", "Ce+4", "Pr",   "Pr+3", "Pr+4", "Nd",
  "Nd+3", "Pm",   "Pm+3", "Sm",   "Sm+3", "Eu",   "Eu+2", "Eu+3", "Gd",   "Gd+3",
  "Tb",   "Tb+3", "Dy",   "Dy+3", "Ho",   "Ho+3", "Er",   "Er+3", "Tm",   "Tm+3",
  "Yb",   "Yb+2", "Yb+3", "Lu",   "Lu+3", "Hf",   "Hf+4", "Ta",   "Ta+5", "W",
  "W+6",  "Re",   "Os",   "Os+4", "Ir",   "Ir+3", "Ir+4", "Pt",   "Pt+2", "Pt+4",
  "Au",   "Au+1", "Au+3", "Hg",   "Hg+1", "Hg+2", "Tl",   "Tl+1", "Tl+3", "Pb",
  "Pb+2", "Pb+4", "Bi",   "Bi+3", "Bi+5", "Po",   "At",   "Rn",   "Fr",   "Ra",
  "Ra+2", "Ac",   "Ac+3", "Th",   "Th+4", "Pa",   "U",    "U+3",  "U+4",  "U+6",
  "Np",   "Np+3", "Np+4", "Np+6", "Pu",   "Pu+3", "Pu+4", "Pu+6", "Am",   "Cm",
  "Bk",   "Cf" };



inline bool is_close(const double a, const double b, const double rel_tol = 1e-9, const double abs_tol = 0.0)
{
	return std::abs(a - b) <= abs_tol * std::max(1.0, rel_tol / abs_tol * std::max(std::abs(a), std::abs(b)));
}

inline bool all_equal(const auto& container)
{
	return std::adjacent_find(container.begin(), container.end(), std::not_equal_to<>()) == container.end();
}

