import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';

class SensorDetails extends StatelessWidget {
  final String sensedval;
  final String stype;
  final IconData icon;
  const SensorDetails(
      {super.key,
      required this.sensedval,
      required this.icon,
      required this.stype});
  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Container(
          decoration: const BoxDecoration(),
          child: Padding(
            padding: const EdgeInsets.only(right: 5.0),
            child: Icon(
              icon,
              size: 50,
              color: Colors.white,
            ),
          ),
        ),
        Wrap(direction: Axis.vertical, children: [
          Text.rich(TextSpan(
            text: '$stype\n', // First part of the text
            style: GoogleFonts.poppins(
              color: Colors.white,
              height: 1,
              fontWeight: FontWeight.w400,
              fontSize: 12,
            ),
            children: <TextSpan>[
              TextSpan(
                text: sensedval, // Second part of the text
                style: GoogleFonts.poppins(
                  height: 1,
                  color: Colors.white,
                  fontWeight: FontWeight.w600, // Different font weight
                  fontSize: 25, // Same font size, or adjust as needed
                ),
              ),
            ],
          )),
        ])
      ],
    );
  }
}
