//
//  ViewController2.swift
//  SHIFT Furniture MOBILE APPLICATION
//  Created by Kyle Skyllingstad
//

// Import necessary kits
import UIKit

// Class for screen 2, ViewController2
class ViewController2: UIViewController {

    
    // Main screen loading function
    override func viewDidLoad() {
        super.viewDidLoad()

    }
    
    
    
    
    
    
    // Functions ////////////////////////////////
    
    
    
    // Function for if Terms of Use button from this screen (VC2) to Terms of Use Screen (VC2TU) tapped
    @IBAction func didTapButton2toTU() {
        
        // Create link to screen TU, VC2TU
        guard let VC = storyboard?.instantiateViewController(identifier: "VC2TU") as? ViewControllerTermsOfUse else {
            return
        }
        
        // Present (go to) VC TU
        VC.modalPresentationStyle = .fullScreen
        present(VC, animated: true)
    }
    
    
    
    // Function for if back button from this screen (VC2) to Privacy Policy Screen (VC2PP) tapped
    @IBAction func didTapButton2toPP() {
        
        // Create link to screen PP, VC2PP
        guard let VC = storyboard?.instantiateViewController(identifier: "VC2PP") as? ViewControllerPrivacyPolicy else {
            return
        }
        
        // Present (go to) VC 1
        VC.modalPresentationStyle = .fullScreen
        present(VC, animated: true)
    }
    
    
    
    // Function for if button from screen 2 (VC2) to screen 3 (VC3) tapped
    @IBAction func didTapButton2to3() {
        
        // Create link to screen 3, or VC3
        guard let VC = storyboard?.instantiateViewController(identifier: "VC3") as? ViewController3 else {
            return
        }
        
        // Present (go to) VC 3
        VC.modalPresentationStyle = .fullScreen
        present(VC, animated: true)
    }
    
    
    
    
    // Function for if back button from this screen (VC2) back to screen 1 (VC1) tapped
    @IBAction func didTapButton2to1() {
        
        // Create link to screen 1, VC1
        guard let VC = storyboard?.instantiateViewController(identifier: "VC1") as? ViewController else {
            return
        }
        
        // Present (go to) VC1
        VC.modalPresentationStyle = .fullScreen
        present(VC, animated: true)
    }
    


}
