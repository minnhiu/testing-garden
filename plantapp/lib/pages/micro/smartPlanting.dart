import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:plantapp/pages/models/Plant.dart';
// import 'package:plantapp/services/notification_service.dart';

class SmartPlanting extends StatefulWidget {
  final Function(int) motorSwitch;
  final Function(int) fanSwitch;
  final Function(int) lightSwitch;
  final int motorpump;
  final int motorlight;
  final int motorfan;
  final Plant selectedPlant;
  final String currentMode; // trạng thái hiện tại của chế độ

  // Thêm các callback để lắng nghe thay đổi
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
  // ignore: library_private_types_in_public_api
  _SmartPlantingState createState() => _SmartPlantingState();
}

class _SmartPlantingState extends State<SmartPlanting> {
  String sensedsoil = "25";
  double minHumidity = 0;
  double maxHumidity = 100;
  String soilMoistureCondition = "Loading...";
  Color soilMoistureColor = Colors.grey;
  String sensedTemp = "0";
  String sensedLight = "0";
  String currentMode = "auto";

  // late DatabaseReference _soilRef;

  @override
  @override
  void initState() {
    super.initState();

    final gardenId = widget.selectedPlant.potId;

    // 🌱 Lắng nghe độ ẩm đất
    FirebaseDatabase.instance.ref('$gardenId/doAmDat').onValue.listen((event) {
      final val = event.snapshot.value;
      if (val != null) {
        setState(() {
          sensedsoil = val.toString();
        });
      }
    });

    // 🌡️ Lắng nghe nhiệt độ
    FirebaseDatabase.instance
        .ref('$gardenId/dhtNhietDo')
        .onValue
        .listen((event) {
      final val = event.snapshot.value;
      if (val != null) {
        setState(() {
          sensedTemp =
              val.toString(); // nhớ khai báo biến sensedTemp nếu chưa có
        });
      }
    });

    // 💡 Lắng nghe ánh sáng
    FirebaseDatabase.instance.ref('$gardenId/anhSang').onValue.listen((event) {
      final val = event.snapshot.value;
      if (val != null) {
        setState(() {
          sensedLight =
              val.toString(); // nhớ khai báo biến sensedLight nếu chưa có
        });
      }
    });

    // 💧 Lắng nghe trạng thái máy bơm
    FirebaseDatabase.instance.ref('$gardenId/mayBom').onValue.listen((event) {
      final val = event.snapshot.value;
      if (val != null) {
        setState(() {
          widget.motorSwitch(val as int);
        });
      }
    });

    // 💨 Lắng nghe trạng thái quạt
    FirebaseDatabase.instance.ref('$gardenId/quat').onValue.listen((event) {
      final val = event.snapshot.value;
      if (val != null) {
        setState(() {
          widget.fanSwitch(val as int);
        });
      }
    });

    // 💡 Lắng nghe trạng thái đèn
    FirebaseDatabase.instance.ref('$gardenId/den').onValue.listen((event) {
      final val = event.snapshot.value;
      if (val != null) {
        setState(() {
          widget.lightSwitch(val as int);
        });
      }
    });

    // 🔄 Lắng nghe trạng thái chung của toàn bộ hệ thống
    FirebaseDatabase.instance.ref('currentMode').onValue.listen((event) {
      final val = event.snapshot.value;
      if (val != null) {
        setState(() {
          currentMode = val.toString(); // nhớ khai báo biến currentMode nếu cần
        });
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        SizedBox(height: 30),
        Text(
          "Devices Control",
          style: GoogleFonts.poppins(
              color: Colors.black, fontWeight: FontWeight.w700, fontSize: 25),
        ),
        SizedBox(height: 22),
        GestureDetector(
          onTap: () {
            // Gửi giá trị mới lên Firebase: nếu đang là auto thì chuyển sang manual
            final newMode = widget.currentMode == "auto" ? "manual" : "auto";
            FirebaseDatabase.instance.ref().child("currentMode").set(newMode);
          },
          child: Container(
            width: 200,
            decoration: BoxDecoration(
              color: widget.currentMode == "auto"
                  ? Colors.green
                  : Color.fromRGBO(203, 203, 203, 1),
              borderRadius: BorderRadius.circular(10),
            ),
            child: Padding(
              padding: const EdgeInsets.all(8.0),
              child: Center(
                child: Text(
                  widget.currentMode == "auto" ? "Auto Mode" : "Manual Mode",
                  style: GoogleFonts.poppins(
                    fontSize: 20,
                    color: widget.currentMode == "auto"
                        ? Colors.white
                        : Colors.black,
                    fontWeight: widget.currentMode == "auto"
                        ? FontWeight.w800
                        : FontWeight.w400,
                  ),
                ),
              ),
            ),
          ),
        ),
        SizedBox(height: 25),
        GestureDetector(
          onTap: () {
            widget.motorSwitch(widget.motorpump == 1 ? 0 : 1);
          },
          child: Container(
            width: 200,
            decoration: BoxDecoration(
              color: widget.motorpump == 1
                  ? Colors.red
                  : Color.fromRGBO(203, 203, 203, 1),
              borderRadius: BorderRadius.circular(10),
            ),
            child: Padding(
              padding: const EdgeInsets.all(8.0),
              child: Center(
                child: Text(
                  widget.motorpump == 1 ? "Pump On" : "Pump Off",
                  style: GoogleFonts.poppins(
                    fontSize: 20,
                    color: widget.motorpump == 1 ? Colors.white : Colors.black,
                    fontWeight: widget.motorpump == 1
                        ? FontWeight.w800
                        : FontWeight.w400,
                  ),
                ),
              ),
            ),
          ),
        ),
        SizedBox(height: 25),
        GestureDetector(
          onTap: () {
            widget.lightSwitch(widget.motorlight == 1 ? 0 : 1);
          },
          child: Container(
            width: 200,
            decoration: BoxDecoration(
              color: widget.motorlight == 1
                  ? Colors.red
                  : Color.fromRGBO(203, 203, 203, 1),
              borderRadius: BorderRadius.circular(10),
            ),
            child: Padding(
              padding: const EdgeInsets.all(8.0),
              child: Center(
                child: Text(
                  widget.motorlight == 1 ? "Light On" : "Light Off",
                  style: GoogleFonts.poppins(
                    fontSize: 20,
                    color: widget.motorlight == 1 ? Colors.white : Colors.black,
                    fontWeight: widget.motorlight == 1
                        ? FontWeight.w800
                        : FontWeight.w400,
                  ),
                ),
              ),
            ),
          ),
        ),
        SizedBox(height: 25),
        GestureDetector(
          onTap: () {
            widget.fanSwitch(widget.motorfan == 1 ? 0 : 1);
          },
          child: Container(
            width: 200,
            decoration: BoxDecoration(
              color: widget.motorfan == 1
                  ? Colors.red
                  : Color.fromRGBO(203, 203, 203, 1),
              borderRadius: BorderRadius.circular(10),
            ),
            child: Padding(
              padding: const EdgeInsets.all(8.0),
              child: Center(
                child: Text(
                  widget.motorfan == 1 ? "Fan On" : "Fan Off",
                  style: GoogleFonts.poppins(
                    fontSize: 20,
                    color: widget.motorfan == 1 ? Colors.white : Colors.black,
                    fontWeight: widget.motorfan == 1
                        ? FontWeight.w800
                        : FontWeight.w400,
                  ),
                ),
              ),
            ),
          ),
        ),
        SizedBox(height: 25),
      ],
    );
  }
}
