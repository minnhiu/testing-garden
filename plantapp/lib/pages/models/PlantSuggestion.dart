// ignore: file_names
class PlantSuggestion {
  final String name;
  final String description;
  final String bestLightCondition;
  final String bestSoilType;
  final String bestWatering;

  PlantSuggestion({
    required this.name,
    required this.description,
    required this.bestLightCondition,
    required this.bestSoilType,
    required this.bestWatering,
  });

  factory PlantSuggestion.fromJson(Map<String, dynamic> suggestion) {
    return PlantSuggestion(
      name: suggestion['name'],
      description: suggestion['details']['description']['value'],
      bestLightCondition: suggestion['details']['best_light_condition'],
      bestSoilType: suggestion['details']['best_soil_type'],
      bestWatering: suggestion['details']['best_watering'],
    );
  }
}
