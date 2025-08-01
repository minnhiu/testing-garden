// ignore: file_names
import 'dart:convert';
import 'package:http/http.dart' as http;
// import 'package:flutter_dotenv/flutter_dotenv.dart';

const String PLANTID_API_KEY =
    "CJsTiXLUFRC2Vc8Wn6aFKVVnqjTdDudMVbfSEv4E2SWH3rabK1";
const String PLANTID_API_URL = "https://plant.id/api/v3";

class PlantIdentifyService {
  static Future<String?> identifyPlant(String base64Image, String path) async {
    final headers = {
      'Api-Key': PLANTID_API_KEY,
      'Content-Type': 'application/json',
    };

    final requestBody = json.encode({
      "images": ["data:image/jpg;base64,$base64Image"],
      "latitude": 49.207,
      "longitude": 16.608,
      "similar_images": true,
    });
    final apiUrl = PLANTID_API_URL;
    final url = Uri.parse("$apiUrl$path");
    final request = http.Request('POST', url);
    request.headers.addAll(headers);
    request.body = requestBody;

    http.StreamedResponse response = await request.send();

    if (response.statusCode == 201) {
      final responseBody = await response.stream.bytesToString();
      //print('Response: $responseBody');
      return responseBody;
    } else {
      // print('Error in service: ${response.reasonPhrase}');
      return null;
    }
  }
}
