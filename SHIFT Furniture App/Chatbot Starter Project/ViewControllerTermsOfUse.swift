//
//  ViewControllerTermsOfUse.swift
//  SHIFT Furniture
//
//  Created by Kyle Skyllingstad on 6/11/20.
//

import UIKit
import WebKit


// Class for the Terms of Use screen
class ViewControllerTermsOfUse: UIViewController {
    
    // Set outlets for entities
    
    // WebView
    @IBOutlet weak var webView: WKWebView!
    
    
    // Main screen loading function
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        
        // Create local URL to call local Terms of Use html file
        if let indexURL = Bundle.main.url(forResource: "TermsOfUseCode", withExtension: "html") {
                    
            // Call the local Terms of Use html file and load it into the WebView object
            self.webView.loadFileURL(indexURL, allowingReadAccessTo: indexURL)
        }
    
    
    }
    
    
    
    
    
    // Function for if back button from this screen (VC2TU) back to screen 2 (VC2) tapped
    @IBAction func didTapButtonTUto2() {
        
        // Create link to screen 2, VC2
        guard let VC = storyboard?.instantiateViewController(identifier: "VC2") as? ViewController2 else {
            return
        }
        
        // Present (go to) VC2
        VC.modalPresentationStyle = .fullScreen
        present(VC, animated: false)
    }
    


}
