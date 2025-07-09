import 'package:flutter/material.dart';
import 'package:flutter_dotenv/flutter_dotenv.dart';
import 'package:firebase_core/firebase_core.dart';
import 'firebase_options.dart';
import 'package:plantapp/pages/start.dart';
import 'package:plantapp/userdets.dart';
import 'package:provider/provider.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();

  // Initialize Firebase with platform-specific options
  await Firebase.initializeApp(
    options: DefaultFirebaseOptions.currentPlatform,
  );
  // Load environment variables
  await dotenv.load(fileName: "assets/env/.env");

  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider(
      create: (context) => UserInfo(),
      child: MaterialApp(
        debugShowCheckedModeBanner: false,
        home: StartPage(),
        theme: ThemeData(
          primarySwatch: Colors.green,
        ),
      ),
    );
  }
}






// ***********
// // // code test connected to firebase
// ***********


// import 'package:flutter/material.dart';
// import 'package:firebase_core/firebase_core.dart';
// import 'package:firebase_database/firebase_database.dart';

// void main() async {
//   WidgetsFlutterBinding.ensureInitialized();
//   await Firebase.initializeApp(
//     options: const FirebaseOptions(
//       apiKey: "AIzaSyCa-0haXGQ1j620ppeOm9HV5Ltdw-JeF6g",
//       authDomain: "smart-garden-64cd4.firebaseapp.com",
//       databaseURL: "https://smart-garden-64cd4-default-rtdb.firebaseio.com",
//       projectId: "smart-garden-64cd4",
//       storageBucket: "smart-garden-64cd4.firebasestorage.app",
//       messagingSenderId: "822512347396",
//       appId: "1:822512347396:web:be26430824583b8d1a0851"
//     ),
//   );
//   runApp(const MyApp());
// }

// class MyApp extends StatelessWidget {
//   const MyApp({super.key});
//   @override
//   Widget build(BuildContext context) {
//     return MaterialApp(
//       title: 'Firebase RTDB Test',
//       home: const HomeScreen(),
//       debugShowCheckedModeBanner: false,
//     );
//   }
// }

// class HomeScreen extends StatefulWidget {
//   const HomeScreen({super.key});
//   @override
//   State<HomeScreen> createState() => _HomeScreenState();
// }

// class _HomeScreenState extends State<HomeScreen> {
//   final DatabaseReference _testRef = FirebaseDatabase.instance.ref("test/value");
//   String _value = "";
//   final TextEditingController _controller = TextEditingController();

//   @override
//   void initState() {
//     super.initState();
//     _testRef.onValue.listen((event) {
//       final data = event.snapshot.value;
//       setState(() {
//         _value = data.toString();
//       });
//     });
//   }

//   void _updateValue() {
//     final newValue = _controller.text.trim();
//     if (newValue.isNotEmpty) {
//       _testRef.set(newValue);
//       _controller.clear();
//     }
//   }

//   @override
//   Widget build(BuildContext context) {
//     return Scaffold(
//       appBar: AppBar(title: const Text("Test Firebase RTDB")),
//       body: Padding(
//         padding: const EdgeInsets.all(20),
//         child: Column(
//           children: [
//             Text("Current value in /test/value:", style: TextStyle(fontSize: 18)),
//             SizedBox(height: 10),
//             Text(_value, style: TextStyle(fontSize: 32, fontWeight: FontWeight.bold)),
//             SizedBox(height: 20),
//             TextField(
//               controller: _controller,
//               decoration: InputDecoration(labelText: "Enter new value"),
//             ),
//             SizedBox(height: 10),
//             ElevatedButton(
//               onPressed: _updateValue,
//               child: const Text("Update Firebase"),
//             )
//           ],
//         ),
//       ),
//     );
//   }
// }
