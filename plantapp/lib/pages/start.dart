import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:plantapp/pages/user/userlogin.dart';

class StartPage extends StatelessWidget {
  const StartPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 20.0, vertical: 20),
            child: Row(
              mainAxisAlignment:
                  MainAxisAlignment.center, // căn giữa ngang toàn bộ
              crossAxisAlignment: CrossAxisAlignment.center,
              children: [
                SizedBox(
                  width: 90,
                  height: 90,
                  child: Image.asset("lib/images/icon.png", fit: BoxFit.cover),
                ),
                SizedBox(
                  width: 20,
                ), // Add some space between the image and text
                SizedBox(
                  // This ensures the Column with text takes the remaining space
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Text(
                        "SMART GARDEN",
                        style: GoogleFonts.poppins(
                          color: const Color.fromARGB(255, 27, 77, 29),
                          fontWeight: FontWeight.w900,
                          height: 1,
                          fontSize: 38,
                        ),
                      ),
                      SizedBox(height: 5),
                      Text(
                        "  Grow Smarter, Live Greener!",
                        style: GoogleFonts.poppins(
                          color: Colors.black,
                          fontWeight: FontWeight.w400,
                          height: 0.9,
                          fontSize: 20,
                        ),
                      ),
                    ],
                  ),
                ),
                SizedBox(width: 20),
              ],
            ),
          ),
          SizedBox(height: 10),
          GestureDetector(
            onTap: () {
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => UserLogin()),
              );
            },
            child: MouseRegion(
              cursor: SystemMouseCursors.click,
              child: Container(
                decoration: BoxDecoration(
                  borderRadius: BorderRadius.circular(40),
                  color: Color.fromRGBO(90, 147, 66, 1),
                ),
                width: MediaQuery.of(context).size.width * 0.9,
                child: Padding(
                  padding: const EdgeInsets.symmetric(
                    vertical: 5,
                    horizontal: 20,
                  ),
                  child: Center(
                    child: Text(
                      "Start Now",
                      style: GoogleFonts.poppins(
                        color: Colors.white,
                        fontWeight: FontWeight.w700,
                        height: 1.5,
                        fontSize: 25,
                      ),
                    ),
                    // You can add onEnter, onExit, etc. if needed
                  ),
                ),
              ),
            ),
          ),
        ],
      ),
    );
  }
}
