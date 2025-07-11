// ignore_for_file: no_leading_underscores_for_local_identifiers, unused_local_variable
import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:plantapp/pages/micro/sensordets.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:plantapp/pages/models/Plant.dart';
import 'package:plantapp/pages/micro/smartPlanting.dart';
// import 'package:flutter_dotenv/flutter_dotenv.dart';

class NodeDetails extends StatefulWidget {
  final Plant plant;
  const NodeDetails({super.key, required this.plant});

  @override
  State<NodeDetails> createState() => _NodeDetailsState();
}

class _NodeDetailsState extends State<NodeDetails> {
  late Plant selectedPlant;
  late DatabaseReference _motorRef;
  late DatabaseReference _fanRef;
  late DatabaseReference _ledRef;

  @override
  void initState() {
    super.initState();
    selectedPlant = widget.plant;
    _motorRef =
        FirebaseDatabase.instance.ref().child('${selectedPlant.potId}/mayBom');
    _fanRef =
        FirebaseDatabase.instance.ref().child('${selectedPlant.potId}/quat');
    _ledRef =
        FirebaseDatabase.instance.ref().child('${selectedPlant.potId}/den');

    _setupListeners(); // Call the setupListeners
    _loadMotorStates();
  }

  void _loadMotorStates() async {
    final motorSnapshot = await _motorRef.get();
    final fanSnapshot = await _fanRef.get();
    final lightSnapshot = await _ledRef.get();

    setState(() {
      motorpump = int.tryParse(motorSnapshot.value.toString()) ?? 0;
      motorfan = int.tryParse(fanSnapshot.value.toString()) ?? 0;
      motorlight = int.tryParse(lightSnapshot.value.toString()) ?? 0;
    });
  }

  final Future<FirebaseApp> _fApp = Firebase.initializeApp();
  String sensedtemp = "0";
  String sensedhumidity = "0";
  String sensedlight = "0";

  String sensedsoil = "0";
  int motorpump = 0;
  int motorlight = 0;
  int motorfan = 0;
  String currentMode = "auto"; // trạng thái hệ thống

  motorSwitch(int valueStateOfMayBom) async {
    setState(() {
      motorpump = valueStateOfMayBom;
      // print(motor);
    });
    await _motorRef.set(motorpump);
  }

  fanSwitch(int valueStateOfFan) async {
    setState(() {
      motorfan = valueStateOfFan;
    });
    await _fanRef.set(motorfan);
  }

  lightSwitch(int valueStateOfLight) async {
    setState(() {
      motorlight = valueStateOfLight;
    });
    await _ledRef.set(motorlight);
  }

  void handleSoilMoistureChange(String condition, Color color) {
    setState(() {
      soilMoistureCondition = condition;
      soilMoistureColor = color;
    });
  }

  // Initialize these variables in initState or as class members
  String temperatureCondition = "Loading...";
  String soilMoistureCondition = "Loading...";
  Color temperatureColor = Colors.grey;
  Color soilMoistureColor = Colors.green;

  void _setupListeners() {
    final potPath = selectedPlant.potId;

    FirebaseDatabase.instance.ref('currentMode').onValue.listen((event) {
      final value = event.snapshot.value;
      if (value != null) {
        setState(() {
          currentMode = value.toString();
        });
      }
    });

    // Temperature
    FirebaseDatabase.instance
        .ref('$potPath/dhtNhietDo')
        .onValue
        .listen((event) {
      final rawData = event.snapshot.value.toString();
      final parsedValue =
          double.tryParse(rawData.replaceAll(RegExp(r'[^\d.]'), ''));
      setState(() {
        sensedtemp = parsedValue?.toStringAsFixed(1) ?? "";
        if (parsedValue != null) {
          if (parsedValue < selectedPlant.nguongNhietDo - 3) {
            temperatureCondition = "Temperature too low";
            temperatureColor = Colors.blue;
          } else if (parsedValue > selectedPlant.nguongNhietDo + 3) {
            temperatureCondition = "Temperature too low";
            temperatureColor = Colors.red;
          } else {
            temperatureCondition = "Suitable temperature for growing";
            temperatureColor = Colors.green;
          }
        }
      });
    });

    // Humidity
    FirebaseDatabase.instance.ref('$potPath/dhtDoAm').onValue.listen((event) {
      final rawData = event.snapshot.value.toString();
      final parsedValue =
          double.tryParse(rawData.replaceAll(RegExp(r'[^\d.]'), ''));
      setState(() {
        sensedhumidity = parsedValue?.toStringAsFixed(1) ?? "";
      });
    });

    // Light intensity
    FirebaseDatabase.instance.ref('$potPath/anhSang').onValue.listen((event) {
      final rawData = event.snapshot.value.toString();
      final parsedValue =
          double.tryParse(rawData.replaceAll(RegExp(r'[^\d.]'), ''));
      setState(() {
        sensedlight = parsedValue?.toStringAsFixed(1) ?? "";
      });
    });

    // Device states
    FirebaseDatabase.instance.ref('$potPath/mayBom').onValue.listen((event) {
      final value = event.snapshot.value;
      if (value != null) {
        setState(() {
          motorpump = value as int;
        });
      }
    });

    FirebaseDatabase.instance.ref('$potPath/quat').onValue.listen((event) {
      final value = event.snapshot.value;
      if (value != null) {
        setState(() {
          motorfan = value as int;
        });
      }
    });

    FirebaseDatabase.instance.ref('$potPath/den').onValue.listen((event) {
      final value = event.snapshot.value;
      if (value != null) {
        setState(() {
          motorlight = value as int;
        });
      }
    });

    // 🌐 System mode (toàn vườn)
    FirebaseDatabase.instance.ref('currentMode').onValue.listen((event) {
      final value = event.snapshot.value;
      if (value != null) {
        setState(() {
          currentMode = value.toString(); // "auto" hoặc "manual"
        });
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        iconTheme: const IconThemeData(
          color: Colors.white,
        ),
        title: Padding(
          padding: const EdgeInsets.all(8.0),
          child: Row(
            children: [
              Text(
                "Smart Garden",
                style: GoogleFonts.poppins(
                    color: Colors.white,
                    fontWeight: FontWeight.w600,
                    fontSize: 25),
              ),
            ],
          ),
        ),
        backgroundColor: const Color.fromRGBO(161, 207, 107, 1),
        actions: const [
          Padding(
            padding: EdgeInsets.all(15.0),
            child: Icon(
              Icons.person,
              color: Colors.white,
            ),
          )
        ],
      ),
      body: SingleChildScrollView(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            const SizedBox(height: 20),
            Center(
              child: Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  Text(
                    "Pot details",
                    style: GoogleFonts.poppins(
                        color: Colors.black,
                        fontWeight: FontWeight.w700,
                        fontSize: 25),
                  ),
                  SizedBox(height: 10),
                  Text(
                    selectedPlant.name,
                    style: GoogleFonts.poppins(
                        color: Colors.black,
                        fontWeight: FontWeight.w600,
                        fontSize: 18),
                  ),
                  const SizedBox(height: 5),
                ],
              ),
            ),
            // Image.asset(selectedPlant.imageUrl,
            //     height: 400, width: 250, fit: BoxFit.scaleDown),
            ClipRRect(
              borderRadius: BorderRadius.circular(15),
              child: Image.asset(
                selectedPlant.imageUrl,
                height: 400,
                width: 250,
                fit: BoxFit.cover,
              ),
            ),
            const SizedBox(height: 20),
            Padding(
                padding:
                    const EdgeInsets.symmetric(vertical: 8.0, horizontal: 20),
                child: Container(
                    decoration: BoxDecoration(
                        borderRadius: BorderRadius.circular(20),
                        gradient: const LinearGradient(
                            begin: Alignment.topCenter,
                            end: Alignment.bottomCenter,
                            colors: [
                              Color.fromRGBO(161, 207, 107, 1),
                              Color.fromRGBO(74, 173, 82, 1)
                            ])),
                    child: FutureBuilder(
                        future: _fApp,
                        builder: (context, snapshot) {
                          if (snapshot.hasError) {
                            return const Padding(
                              padding: EdgeInsets.all(8.0),
                              child: Text("Firebase Error! Try again later."),
                            );
                          } else if (snapshot.hasData) {
                            return sensorcontent();
                          } else {
                            return const CircularProgressIndicator();
                          }
                        }))),
            ElevatedButton(
                // TO DO: Add model feature here
                onPressed: () {
                  // runModel(double.parse(sensedtemp),
                  //     double.parse(sensedhumidity), sensedrainfall.toDouble());
                  showInformation(context, selectedPlant.id);
                },
                style: ElevatedButton.styleFrom(
                  shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(5.0),
                  ),
                  backgroundColor: const Color.fromRGBO(74, 173, 82, 1),
                ),
                child: Text(
                  "Get Plant Information",
                  style: GoogleFonts.poppins(
                    height: 1,
                    color: Colors.white,
                    fontWeight: FontWeight.normal, // Different font weight
                    fontSize: 17, // Same font size, or adjust as needed
                  ),
                )),
            const SizedBox(height: 10),

            SmartPlanting(
              motorSwitch: motorSwitch,
              fanSwitch: fanSwitch,
              lightSwitch: lightSwitch,
              motorpump: motorpump,
              motorlight:
                  motorlight, // Provide an appropriate value or variable here
              motorfan:
                  motorfan, // Provide an appropriate value or variable here
              selectedPlant: selectedPlant,
              onSoilMoistureChange: handleSoilMoistureChange,
              currentMode: currentMode,
            )
          ],
        ),
      ),
    );
  }

  Widget sensorcontent() {
    return Column(
      children: [
        Padding(
          padding: const EdgeInsets.symmetric(horizontal: 20.0),
          child: Column(
            children: [
              // Container(
              //   padding: const EdgeInsets.all(10),
              //   decoration: BoxDecoration(
              //     color: temperatureColor,
              //     borderRadius: BorderRadius.circular(10),
              //   ),
              //   child: Text(
              //     temperatureCondition,
              //     style: GoogleFonts.poppins(
              //       fontSize: 16,
              //       fontWeight: FontWeight.w600,
              //       color: Colors.white,
              //     ),
              //     textAlign: TextAlign.center,
              //   ),
              // ),
              const SizedBox(height: 20),
              Container(
                padding: const EdgeInsets.all(10),
                decoration: BoxDecoration(
                  color: soilMoistureColor,
                  borderRadius: BorderRadius.circular(10),
                ),
                child: Text(
                  "Environmental Parameters",
                  style: GoogleFonts.poppins(
                    fontSize: 16,
                    fontWeight: FontWeight.w600,
                    color: Colors.white,
                  ),
                  textAlign: TextAlign.center,
                ),
              ),
            ],
          ),
        ),
        // const SizedBox(height: 20),
        GridView.count(
          crossAxisCount: 2,
          childAspectRatio: 2,
          shrinkWrap: true,
          padding: const EdgeInsets.symmetric(
            horizontal: 25,
            vertical: 12,
          ),
          physics: const NeverScrollableScrollPhysics(),
          children: [
            SensorDetails(
              sensedval: "$sensedtemp°C",
              icon: Icons.thermostat,
              stype: "Temperature",
            ),
            SensorDetails(
              sensedval: "$sensedhumidity%",
              icon: Icons.water,
              stype: "Humidity",
            ),
            SensorDetails(
              sensedval: sensedlight,
              icon: Icons.lightbulb,
              stype: "Light Intensity",
            ),
          ],
        ),
      ],
    );
  }
}

void showInformation(BuildContext context, int plantId) {
  // Tìm kiếm cây dựa trên id
  Plant plant = plants.firstWhere((plant) => plant.id == plantId);

  // Hiển thị thông tin cây trong AlertDialog
  showDialog(
    context: context,
    builder: (context) {
      return AlertDialog(
        actions: [
          TextButton(
            onPressed: () {
              Navigator.of(context).pop();
            },
            child: Text('OK'),
          ),
        ],
        title: Text(
          'Plant Information',
          style: TextStyle(fontWeight: FontWeight.w600, fontSize: 20),
        ),
        content: SingleChildScrollView(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            mainAxisSize: MainAxisSize.min,
            children: [
              Image.asset(plant.imageUrl),
              SizedBox(height: 10),
              Text(
                plant.description,
                style: TextStyle(fontSize: 16),
              ),
              SizedBox(height: 10),
              Text(
                plant.name.toUpperCase(),
                style: TextStyle(
                  fontWeight: FontWeight.w900,
                  fontSize: 24,
                  color: Colors.green,
                ),
              ),
              SizedBox(height: 10),
              // Lời khuyên chăm sóc cây
              Text(
                "Lời khuyên chăm sóc:",
                style: TextStyle(fontWeight: FontWeight.w700, fontSize: 18),
              ),
              SizedBox(height: 5),
              Text(
                "Nhiệt độ:",
                style: TextStyle(
                  fontWeight: FontWeight.bold,
                  fontSize: 16,
                  color: Colors.green,
                ),
              ),
              Text(
                plant.temperatureAdvice,
                style: TextStyle(fontSize: 16),
              ),
              SizedBox(height: 5),
              Text(
                "Độ ẩm đất:",
                style: TextStyle(
                  fontWeight: FontWeight.bold,
                  fontSize: 16,
                  color: Colors.green,
                ),
              ),
              Text(
                plant.soilMoistureAdvice,
                style: TextStyle(fontSize: 16),
              ),
              SizedBox(height: 5),
              Text(
                "Chu kỳ tưới cây:",
                style: TextStyle(
                  fontWeight: FontWeight.bold,
                  fontSize: 16,
                  color: Colors.green,
                ),
              ),
              Text(
                plant.wateringCycleAdvice,
                style: TextStyle(fontSize: 16),
              ),
              SizedBox(height: 5),
              Text(
                "Ánh sáng:",
                style: TextStyle(
                  fontWeight: FontWeight.bold,
                  fontSize: 16,
                  color: Colors.green,
                ),
              ),
              Text(
                plant.lightAdvice,
                style: TextStyle(fontSize: 16),
              ),
            ],
          ),
        ),
      );
    },
  );
}
