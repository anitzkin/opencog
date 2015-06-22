
(define conj_and
    (BindLink
        (VariableList
            (TypedVariableLink
                (VariableNode "$a-parse")
                (TypeNode "ParseNode")
            )
            (TypedVariableLink
                (VariableNode "$word1")
                (TypeNode "WordInstanceNode")
            )
            (TypedVariableLink
                (VariableNode "$word2")
                (TypeNode "WordInstanceNode")
            )
            (TypedVariableLink
                (VariableNode "$pos")
                (TypeNode "DefinedLinguisticConceptNode")
            )
        )
        (AndLink
            (WordInstanceLink
                (VariableNode "$word1")
                (VariableNode "$a-parse")
            )
            (WordInstanceLink
                (VariableNode "$word2")
                (VariableNode "$a-parse")
            )
            (PartOfSpeechLink
   		(VariableNode "$word1")
   		(VariableNode "$pos")
	    )
	    (EvaluationLink
   		(PrepositionalRelationshipNode "conj_and")
   		(ListLink
      			(VariableNode "$word1")
      			(VariableNode "$word2")
   		)
	    )
        )
        (ExecutionOutputLink
       	   (GroundedSchemaNode "scm: pre-conj_and-rule")
       	      (ListLink
       	         (VariableNode "$word1")
		 (VariableNode "$word2")
       	         (VariableNode "$pos")
            )
        )
    )
)

(InheritanceLink (stv 1 .99) (ConceptNode "conj_and-Rule") (ConceptNode "Rule"))

(ReferenceLink (stv 1 .99) (ConceptNode "conj_and-Rule") conj_and)

; This is function is not needed. It is added so as not to break the existing
; r2l pipeline.
(define (pre-conj_and-rule word1 word2 pos)
    	(and-rule (word-inst-get-word-str word1) (cog-name word1)
	(and-rule (word-inst-get-word-str word2) (cog-name word2)
	(word-inst-get-word-str pos) (cog-name pos)
              
    )
)


