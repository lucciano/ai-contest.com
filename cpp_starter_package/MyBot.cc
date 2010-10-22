#define SYSLOG 1
#include <iostream>
#include "PlanetWars.h"
#include <cmath>

#ifdef SYSLOG
#include <syslog.h>
#endif
//Rutinas a implementar:
int FindWeeakPlanet(std::vector<Planet> planets){
  int source = -1;
  int source_num_ships = 999999;
  for (int i = 0; i < planets.size(); ++i) {
    const Planet& p = planets[i];
    int score = p.NumShips();
    if (score < source_num_ships){
      source = p.PlanetID();
      source_num_ships = p.NumShips();
    }
  }
  return source;
}

//Encontrar el planeta mas fuerte
int FindStrongPlannet(std::vector<Planet> planets){
  int source = -1;
  int source_num_ships = 0;
  for (int i = 0; i < planets.size(); ++i) {
    const Planet& p = planets[i];
    int score = p.NumShips();
    if (score > source_num_ships) {
      source = p.PlanetID();
      source_num_ships = p.NumShips();
    }
  }
  return source;
}
//Ataque coordinado, enviar naves, tantas como sea posibles, desde los planetas mas cercanos
void coordinateAttack(PlanetWars & pw, int planet_target, int attacak_force){
  if(attacak_force > 0){
    attacak_force += 1;
    std::vector<Planet> my_planets = pw.MyPlanets();
    int max_force = 0;
    for(int i = 0; i < my_planets.size(); ++i){
      max_force += my_planets[i].NumShips();
    }
    if(max_force<attacak_force) return;
    for(int i = 0; i < my_planets.size(); ++i){
      int source = FindStrongPlannet(my_planets);
      if(source >= 0){
        const Planet& sourcePlanet = pw.GetPlanet(source);
        int distance = pw.Distance(source, planet_target);
        if(pw.GetPlanet(planet_target).Owner() == 2){
          attacak_force += pw.GetPlanet(planet_target).GrowthRate() * distance;
        }
        if(attacak_force < pw.GetPlanet(source).NumShips()){
          pw.IssueOrder(source, planet_target, attacak_force);
          return;
        } else if(sourcePlanet.NumShips() > sourcePlanet.GrowthRate()){
          pw.IssueOrder(source, planet_target, pw.GetPlanet(source).GrowthRate());
          attacak_force -= sourcePlanet.GrowthRate();
        }
      }
    }
  }
}
//Encontrar el planeta mas cercano a un punto
//Encontrar centro de masa
//Encontrar fleets que van a un planeta
//Simular un juego de fleets contra un planeta y ver el resultado(como un planeta)
Planet SimulateFleets(Planet p, std::vector<Fleet> fleets){
  Planet r(p.PlanetID(), p.Owner(), p.NumShips(), p.GrowthRate(), p.X(), p.Y());
  int turnos = 0;
  for(int j = 0; j < fleets.size(); j++){
     const Fleet& f = fleets[j];
     if(turnos < f.TurnsRemaining() && p.PlanetID() == f.DestinationPlanet()){
       turnos = f.TurnsRemaining();
     }
  }
  for(int k = 0; k <= turnos; k++){
    int ships = r.NumShips();
    for(int j = 0; j < fleets.size(); j++){
      const Fleet& f = fleets[j];
      if(f.TurnsRemaining() == k && p.PlanetID() == f.DestinationPlanet()){
        if(f.Owner() == p.Owner()){
          ships += f.NumShips();
        }else{
          ships -= f.NumShips();
        }
      }
    }
    if(ships < 0){
	r.Owner(p.Owner()==1?2:1);
    }
    r.NumShips((int)std::abs((double)ships));
    if(r.Owner() != 0) r.AddShips(r.GrowthRate());
  }
  return r;
}

// The DoTurn function is where your code goes. The PlanetWars object contains
// the state of the game, including information about all planets and fleets
// that currently exist. Inside this function, you issue orders using the
// pw.IssueOrder() function. For example, to send 10 ships from planet 3 to
// planet 8, you would say pw.IssueOrder(3, 8, 10).
//
// http://www.ai-contest.com/resources.
void DoTurn(PlanetWars& pw) {

  if(pw.MyPlanets().size() == 1 && pw.MyFleets().size() == 0 &&
     pw.EnemyPlanets().size() == 1 ){//First round
    
    std::vector<Planet> my_planets = pw.MyPlanets();
    std::vector<Planet> enemy_planets = pw.EnemyPlanets();
    Planet& my_planet = my_planets[0];
    Planet& enemy_planet = enemy_planets[0];
    int distance = pw.Distance(my_planet.PlanetID(), enemy_planet.PlanetID());
#ifdef SYSLOG
  syslog (LOG_NOTICE, "First turn!!!");

    syslog( LOG_NOTICE, "Coordinets for planet %d are (%f, %f)", my_planet.PlanetID(), my_planet.X(), my_planet.Y());
    syslog (LOG_NOTICE, "Distancia : %d, MyGrowRate = %d, EnenmyGrowRate = %d", 
		pw.Distance(my_planet.PlanetID(), enemy_planet.PlanetID()), 
		my_planet.GrowthRate(), my_planet.GrowthRate());
#endif
    if(distance * my_planet.GrowthRate() < my_planet.NumShips() and my_planet.NumShips() >= distance * enemy_planet.GrowthRate() + 1){//Estamos peligrosamente cerca del enemigo
      pw.IssueOrder(my_planet.PlanetID(), enemy_planet.PlanetID(), distance * enemy_planet.GrowthRate() + 1);
    }else{ //Tratamos de conquistar la mayor canitdad de planetas en este turno.
      return;
    }
  }

  //Encontramos nuestro centro de masa de todos nuestros planetas incluyendo crecimiento
  std::vector<Planet> my_planets = pw.MyPlanets();
  int ship_count[my_planets.size()];
  double cm_x, cm_y, sum_growth_rate;
  for (int i = 0; i < my_planets.size(); ++i) {
    const Planet& p = my_planets[i];
    //sum_growth_rate += p.GrowthRate();
    //cm_x += p.GrowthRate() * p.X();
    //cm_y += p.GrowthRate() * p.Y();

    cm_x += p.X();
    cm_y += p.Y();
    sum_growth_rate += 1;
  }
  cm_x = cm_x / sum_growth_rate;
  cm_y = cm_y / sum_growth_rate;

  //Ciclo de defensa
  //Sobre las flotas enemigas que atacan uno de nuestros planetas tratamos de bloquearlas
  for (int i = 0; i < my_planets.size(); ++i) {
    Planet& p = my_planets[i];
    Planet result = SimulateFleets(p, pw.Fleets());
    if(result.Owner() == 2){
      int num_ships = result.NumShips()+1;
      coordinateAttack( pw, p.PlanetID(), result.NumShips()+1);
    }
  //    else if(result.Owner() == 1){
  //    p.NumShips(result.NumShips());
  //  }
  }

  //if(pw.MyFleets().size() >= 1) return;
  //Ciclo de ataque (expansion)
  //Buscar el planeta más cercano al cento de masa y darle
  std::vector<Planet> not_my_planets = pw.NotMyPlanets();
  int dest = -1;
  //double best_distance = 99999;
  //for(int i = 0; i < not_my_planets.size(); i++){
  //  const Planet& p = not_my_planets[i];
  //  double dx = cm_x - p.X();
  //  double dy = cm_y - p.Y();
  //  double distance = (int)ceil(sqrt(dx * dx + dy * dy));
  //  Planet result = SimulateFleets(p, pw.Fleets());
  //  if(distance < best_distance and result.Owner() != 1){ 
  //    best_distance = distance;
  //    dest = p.PlanetID();
  //  }
  //}

  int valoracion = 9999999;
  for(int i = 0; i < not_my_planets.size(); i++){
    Planet &p_i = not_my_planets[i];
    int valoracion_i = 0;
    for(int j = 0; j < my_planets.size(); ++j){
      Planet &p_j = my_planets[j];
      valoracion_i += pw.Distance(p_j.PlanetID(), p_i.PlanetID()) * 2/ p_i.GrowthRate();
    }
    if(valoracion_i < valoracion){
       valoracion = valoracion_i;
       dest = p_i.PlanetID();
    }
  }

  if(dest >= 0){
    Planet result = SimulateFleets(pw.GetPlanet(dest), pw.Fleets());
    int num_ships = result.NumShips();
    //if(pw.GetPlanet(dest).Owner() == 2) num_ships = num_ships * 1.5;
    coordinateAttack(pw, dest, num_ships);
  }
}

// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main(int argc, char *argv[]) {
  std::string current_line;
  std::string map_data;
#ifdef SYSLOG
  openlog ("MyBot.log", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
  syslog (LOG_NOTICE, "Program started by User %d", getuid ());
#endif

  while (true) {
    int c = std::cin.get();
    current_line += (char)c;
    if (c == '\n') {
      if (current_line.length() >= 2 && current_line.substr(0, 2) == "go") {
        PlanetWars pw(map_data);
        map_data = "";
        DoTurn(pw);
	pw.FinishTurn();
      } else {
        map_data += current_line;
      }
      current_line = "";
    }
  }

  return 0;
}
