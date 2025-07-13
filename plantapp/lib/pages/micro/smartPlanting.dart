import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:plantapp/pages/models/Plant.dart';

class SmartPlanting extends StatefulWidget {
  final Function(int) motorSwitch;
  final Function(int) fanSwitch;
  final Function(int) lightSwitch;
  final int motorpump;
  final int motorlight;
  final int motorfan;
  final Plant selectedPlant;
  final String currentMode;
  final Function(String, Color) onSoilMoistureChange;

  const SmartPlanting({
    super.key,
    required this.motorSwitch,
    required this.fanSwitch,
    required this.lightSwitch,
    required this.motorpump,
    required this.motorlight,
    required this.motorfan,
    required this.currentMode,
    required this.selectedPlant,
    required this.onSoilMoistureChange,
  });

  @override
  _SmartPlantingState createState() => _SmartPlantingState();
}

class _SmartPlantingState extends State<SmartPlanting> {
  String sensedsoil = "25";
  String sensedTemp = "0";
  String sensedLight = "0";
  String currentMode = "auto";

  @override
  void initState() {
    super.initState();
    currentMode = widget.currentMode;
    final gardenId = widget.selectedPlant.potId;

    FirebaseDatabase.instance.ref('$gardenId/doAmDat').onValue.listen((event) {
      final val = event.snapshot.value;
      if (val != null) {
        setState(() {
          sensedsoil = val.toString();
        });
      }
    });

    FirebaseDatabase.instance
        .ref('$gardenId/dhtNhietDo')
        .onValue
        .listen((event) {
      final val = event.snapshot.value;
      if (val != null) {
        setState(() {
          sensedTemp = val.toString();
        });
      }
    });

    FirebaseDatabase.instance.ref('$gardenId/anhSang').onValue.listen((event) {
      final val = event.snapshot.value;
      if (val != null) {
        setState(() {
          sensedLight = val.toString();
        });
      }
    });

    FirebaseDatabase.instance.ref('$gardenId/mayBom').onValue.listen((event) {
      final val = event.snapshot.value;
      if (val != null) {
        widget.motorSwitch(val as int);
      }
    });

    FirebaseDatabase.instance.ref('$gardenId/quat').onValue.listen((event) {
      final val = event.snapshot.value;
      if (val != null) {
        widget.fanSwitch(val as int);
      }
    });

    FirebaseDatabase.instance.ref('$gardenId/den').onValue.listen((event) {
      final val = event.snapshot.value;
      if (val != null) {
        widget.lightSwitch(val as int);
      }
    });

    FirebaseDatabase.instance.ref('currentMode').onValue.listen((event) {
      final val = event.snapshot.value;
      if (val != null) {
        setState(() {
          currentMode = val.toString();
        });
      }
    });
  }

  // Hàm xác định màu nút
  Color getDeviceColor(bool isOn, bool isAutoMode) {
    if (isAutoMode) return Colors.grey;
    return isOn ? Colors.green : Colors.red;
  }

  Widget buildDeviceButton({
    required String labelOn,
    required String labelOff,
    required int value,
    required VoidCallback onTap,
    required bool isAutoMode,
  }) {
    bool isOn = value == 1;
    return GestureDetector(
      onTap: isAutoMode ? null : onTap,
      child: Container(
        width: 200,
        decoration: BoxDecoration(
          color: getDeviceColor(isOn, isAutoMode),
          borderRadius: BorderRadius.circular(10),
        ),
        child: Padding(
          padding: const EdgeInsets.all(8.0),
          child: Center(
            child: Text(
              isOn ? labelOn : labelOff,
              style: GoogleFonts.poppins(
                fontSize: 20,
                color: Colors.white,
                fontWeight: FontWeight.w600,
              ),
            ),
          ),
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    final isAuto = currentMode == "auto";

    return Column(
      children: [
        SizedBox(height: 30),
        Text(
          "Devices Control",
          style: GoogleFonts.poppins(
            color: Colors.black,
            fontWeight: FontWeight.w700,
            fontSize: 25,
          ),
        ),
        SizedBox(height: 22),

        // Nút chuyển chế độ
        GestureDetector(
          onTap: () {
            final newMode = currentMode == "auto" ? "manual" : "auto";
            FirebaseDatabase.instance.ref().child("currentMode").set(newMode);
          },
          child: Container(
            width: 200,
            decoration: BoxDecoration(
              color: isAuto ? Colors.green : Colors.grey,
              borderRadius: BorderRadius.circular(10),
            ),
            child: Padding(
              padding: const EdgeInsets.all(8.0),
              child: Center(
                child: Text(
                  isAuto ? "Auto Mode" : "Manual Mode",
                  style: GoogleFonts.poppins(
                    fontSize: 20,
                    color: isAuto ? Colors.white : Colors.black,
                    fontWeight: isAuto ? FontWeight.w800 : FontWeight.w400,
                  ),
                ),
              ),
            ),
          ),
        ),
        SizedBox(height: 25),

        // Nút máy bơm
        buildDeviceButton(
          labelOn: "Pump On",
          labelOff: "Pump Off",
          value: widget.motorpump,
          onTap: () => widget.motorSwitch(widget.motorpump == 1 ? 0 : 1),
          isAutoMode: isAuto,
        ),
        SizedBox(height: 25),

        // Nút đèn
        buildDeviceButton(
          labelOn: "Light On",
          labelOff: "Light Off",
          value: widget.motorlight,
          onTap: () => widget.lightSwitch(widget.motorlight == 1 ? 0 : 1),
          isAutoMode: isAuto,
        ),
        SizedBox(height: 25),

        // Nút quạt
        buildDeviceButton(
          labelOn: "Fan On",
          labelOff: "Fan Off",
          value: widget.motorfan,
          onTap: () => widget.fanSwitch(widget.motorfan == 1 ? 0 : 1),
          isAutoMode: isAuto,
        ),
        SizedBox(height: 25),
      ],
    );
  }
}
