#pragma once

#include <string>
#include <vector>
#include <variant>
#include <functional>


// Common

class Time;

// Map

class Quad;
class Lot;
class Block;

class Element;
class Chunk;

enum FACE_DIRECTION : int;

class Stair;
class Ceiling;
class Corridor;
class Single;
class Row;
class Layout;

class Terrain;
class TerrainFactory;
class Roadnet;
class RoadnetFactory;
class Zone;
class ZoneFactory;
class Building;
class BuildingFactory;
class Component;
class ComponentFactory;
class Room;
class RoomFactory;

class Map;

// Populace

class Experience;
class EducationExperience;
class EmotionExperience;
class JobExperience;

class Person;

class Commute;

class Asset;
class AssetFactory;
class Name;
class NameFactory;
class Scheduler;
class SchedulerFactory;

class Populace;

// Society

class Calendar;
class CalendarFactory;
class Job;
class JobFactory;
class Organization;
class OrganizationFactory;

class Society;

// Story


class Condition;
class Section;
class Event;
class Change;
class Dialog;
class Milestone;

using ValueType = std::variant<int, double, bool, std::string>;
using Action = std::variant<Dialog*, Change*>;

class Script;
class ScriptFactory;

class Story;

// Industy;

class Product;
class ProductFactory;
class Storage;
class StorageFactory;
class Manufacture;
class ManufactureFactory;

class Industry;

// Traffic

class Vehicle;
class VehicleFactory;
class Station;
class StationFactory;
class Route;
class RouteFactory;

class Traffic;

// Player

class Skill;
class SkillFactory;

class Player;

