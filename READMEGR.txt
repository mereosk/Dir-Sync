﻿ΠΑΝΕΠΙΣΤΗΜΙΟ ΑΘΗΝΩΝ 
Τμήμα Πληροφορικής και Τηλεπικοινωνιών 
4η Εργασία - Τμήμα: Περιττών Αριθμών Μητρώου 
Κ22: Λειτουργικά Συστήματα – Χειμερινό Εξάμηνο ΄20


Κωνσταντίνος Μερεός  sdi1700085








________________






Execution and Separate Compilation
Το πρόγραμμά μου αποτελείται από τρεις φακέλους, include, modules, programs. Στον φάκελο programs υπάρχει το κύριο πρόγραμμα quic.c. Το προηγούμενο περιέχει τη main, καθώς και τις κύριες συναρτήσεις copyDir και  deleteDirOrFiles. Ο φάκελος modules περιέχει συναρτήσεις που έχουν ρόλο επικουρικό στο πρόγραμμα quic.c. Η αρχικοποίηση αυτών των συναρτήσεων γίνεται στο φάκελο include. Τέλος στον φάκελο programs υπάρχει και το Makefile από το οποίο μπορεί να γίνουν οι εξείς λειτουργίες:
* Μπορεί να δημιουργήσει το εκτελέσιμο αρχείο quic.
$make
* Μπορεί να δημιουργήσει και να τρέξει το εκτελέσιμο quic. Αν θέλω να αλλάξω τα arguments που περνάω στο terminal πάω στο Makefile και αλλάζω τις τιμές στη μεταβλητή ARGS.
$make run
* Μπορεί να τρέξει με valgrind για debugging με το :
$make valgrind
* Μπορεί να διαγράψει όλα τα εκτελέσιμα και αντικειμενικά αρχεία του project με το:
$make clean
Όπως είπα στο δεύτερο bullet, μέσα στο Makefile υπάρχει μια μεταβλητή ARGS η οποία δίνει τα ορίσματα στην γραμμή εντολών. Επειδή έχω βάλει το destination directory να δημιουργείται στο “~/” θα πρότεινα να το αλλάξετε για να το δοκιμάσετε στο path της επιλογής σας.
main
Η main αποτελεί την καρδιά του προγράμματος. Η προηγούμενη τσεκάρει αν τα ορίσματα είναι επιτρεπτά. Για αυτό το λόγο καλεί τη συνάρτηση checkCommandArguments, η οποία τσεκάρει αν έχουν μπει σωστά οι σημαίες, αν είναι ίδιες, αν το source directory υπάρχει και είναι όντως φάκελος και τέλος αν τα ορίσματα είναι μέσα στα επιτρεπτά όρια. Στη συνέχεια, η συνάρτηση καλεί την copyDir για να αντιγραφεί το source directory στο destination directory και την deleteDirOrFiles για να ψάξει αν έχει διαγραφεί κάποιος φάκελος/αρχείο από το origin directory το οποίο πρέπει να διαγραφεί στο destination. Τέλος εκτελεί τις ζητούμενες εκτυπώσεις μέσω της συνάρτησης printLogs.


copyDir
Αρχικά η συνάρτηση ανοίγει τα directories, στην περίπτωση που δεν υπάρχει το destination directory το δημιουργεί. Στη συνέχεια, το source directory, εκτελεί μία επανάληψη σε όλα του τα στοιχεία με τη βοήθεια της readdir. Φτιάχνει κατάλληλα τα ονόματα των αρχείων/φακέλων που διαβάζει και παίρνει τα στατιστικά τους και στο source directory αλλά και στο destination. Όταν εκτελεστεί το stat για το dest directory στοιχείο υπάρχουν δύο επιλογές.
* Να επιστρέψει αριθμό μικρότερο του μηδενός και να ορίσει το errno. Αν το προηγούμενο είναι ENNOENT, σημαίνει ότι δεν υπάρχει στο στοιχείο στο destination directory άρα πρέπει να το δημιουργήσει. Σε οποιαδήποτε άλλη περίπτωση απλά βγάζει error. Συνεχίζοντας, όταν βγάλει ENNOENT χωρίζεται πάλι σε δύο μέρη:
1. Το στοιχείο είναι αρχείο. Σε αυτήν την περίπτωση, δημιουργείται ένα αρχείο μέσα στο destination directory και αντιγράφονται τα δεδομένα του αρχείου από το source directory. Επίσης παίρνει τα permissions του αρχείου στο source dir και βάζει τα ίδια στο καινούργιο αρχείο για να είναι πανομοιότυπα. Τα δικαιώματα τα παίρνει μέσω της συνάρτησης stat η οποία δίνει το mode. Όμως, επειδή αυτά βρίσκονται μέσα στα τελευταία 9 bits χρησιμοποιείται μάσκα για να τα απομονώσει.
2. Το στοιχείο είναι φάκελος. Σε αυτήν την περίπτωση δημιουργείται ένας φάκελος στο destination directory με τα δικαιώματα του φακέλου από το source directory (πάλι με stat).
* Να επιστρέψει αριθμό μεγαλύτερο του μηδενός. Σε αυτήν την περίπτωση υπάρχει ένα στοιχείο με αυτό το όνομα στο destination directory. Εδώ, πρέπει να γίνει έλεγχος αν αυτά τα δύο είναι ΙΔΙΑ. Έτσι συγκρίνονται τα δύο στοιχεία.
1. Τα δύο στοιχεία είναι διαφορετικού είδους (πχ το ένα φάκελος και το άλλο αρχείο). 
1. Αν στο source directory, το στοιχείο είναι φάκελος(dirSource), σημαίνει ότι στο destination directory το στοιχείο είναι αρχείο. Έτσι διαγράφεται το αρχείο, δημιουργείται ο φάκελος(dirDest)  και αναδρομικά καλείται η συνάρτηση copyDir, για να αντιγραφούν όλα τα περιεχόμενα του φακέλου dirSource μέσα στο νέο φάκελο dirDest.
2. Αν συμβαίνει το αντίθετο, διαγράφεται ο φάκελος στο destination directory και αντιγράφεται το αρχείο.
2. Τα δύο στοιχεία είναι directories. Σε αυτήν την περίπτωση καλείται αναδρομικά η copyDir, για να διαπιστωθεί αν όντως, οι δύο φάκελοι και τα περιεχόμενά τους, είναι ίδια.
3. Τα δύο στοιχεία είναι αρχεία αλλά διαφερουν στο μέγεθός τους. Αυτό μπορεί να αναδειχθεί με τη βοήθεια της stat, η οποία μας προσφέρει το μέγεθος των αρχείων. Σε αυτήν την περίπτωση το αρχείο fileDest γίνεται truncate και γίνεται πάλι η αντιγραφή του fileDest από το fileSource.
4. Τα δύο στοιχεία είναι αρχεία αλλά το fileDest έχει ημερομηνία τροποποίησης μικρότερη από αυτή του fileSource. Αυτό σημαίνει ότι έχει γίνει κάποια τροποποίηση στο fileSource. Αυτό μπορεί να αναδειχθεί με τη βοήθεια της stat η οποία μας προσφέρει τη συγκεκριμένη ημερομηνία. Σε αυτήν την περίπτωση το αρχείο fileDest γίνεται truncate και γίνεται πάλι η αντιγραφή του fileDest από το fileSource.
5. Τα δύο αρχεία είναι ίδια όπου συνεχίζεται η επανάληψη σε επόμενο στοιχείο.


Κατά τη διάρκεια όλων αυτών υπάρχουν κάποιες μεταβλητές, οι οποίες εκτελούν τις μετρήσεις που θα εκτυπωθούν στο τέλος (αριθμός αρχείων κλπ).






deleteDirOrFiles
Ο λόγος της υλοποίησης την παραπάνω συνάρτησης είναι να δούμε αν έχει διαγραφεί κάποιο στοιχείο από το source dictionary. Για αυτό το λόγο η deleteDirOrFiles δρα αντίθετα από την copyDir. Συγκεκριμένα, κάνει traverse το destination dictionary και τσεκάρει αν κάποιο στοιχείο του, δεν υπάρχει στο source dictionary.
* Αν δεν υπάρχει σημαίνει ότι έχει διαγραφεί. Άρα πρέπει να διαγραφεί και το προκείμενο στοιχείο στο destination dictionary. Αν είναι αρχείο φτάνει ένα απλό remove. Αν, όμως, είναι φάκελος πρέπει να διαγραφεί όλη η δομή του από μέσα προς τα έξω. Σε αυτό βοηθάει η nftw, την οποία διάλεξα έναντι της απλής ftw, γιατί μου δίνει την βοηθητική σημαία FTW_DEPTH.
* Αν υπάρχει και είναι αρχείο δεν γίνεται τίποτε. Όμως αν υπάρχει και είναι φάκελος, δεν είναι σίγουρο ότι τα περιεχόμενά του δεν έχουν το ίδιο πρόβλημα. Για αυτό το λόγο καλείται αναδρομικά η deleteDirOrFiles.


Να προσθέσω ότι το παραπάνω έχει κακή πολυπλοκότητα αλλά σύμφωνα με μια ανάρτηση στο piazza είπατε ότι δεν μας ενδιαφέρει κάτι τέτοιο στην προκειμένη περίπτωση.




Ευχαριστώ για το χρόνο και την υπομονή σας.
